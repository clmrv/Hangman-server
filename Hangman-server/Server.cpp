//
//  Server.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include "Server.hpp"
#include "Connection/Message.hpp"

Server::Server(int port): config("config") {    
    newConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (newConnectionSocket == -1) {
        printf("socked failed\n");
        return;
    }
    const int one = 1;
    setsockopt(newConnectionSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
   
    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(config.port);
    servAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(newConnectionSocket, (sockaddr*)&servAddr, sizeof(servAddr) ) == -1) {
        printf("Binding failed: %d\n", errno);
        return;
    }

    listen(newConnectionSocket, 5);     // second arg - how many can wait to be accepted (in queue)
    this->eventLoop();
}

void Server::eventLoop() {
    events.push_back({ newConnectionSocket, POLLIN });

    while (true) {

        if(poll(events.data(), (unsigned int)events.size(), 10) > 0) {
            for(auto const &event : events) {

                if(event.fd == newConnectionSocket) {

                    // Accept new connection
                    if(event.revents & POLLIN) {
                        connect();
                    }

                } else {

                    // Read from connection
                    if(event.revents & POLLIN) {
                        connections[event.fd].read();
                    }

                    // Write to connection
                    if(event.revents & POLLOUT) {
                        connections[event.fd].write();
                    }

                    // Disconnect
                    if(event.revents & POLLHUP) {
                        disconnect(event.fd);
                    }

                }
            }

            handleMessages();
        }

        // Mark whether poll should wait for output event
        for(auto& event : events) {
            if(event.fd != newConnectionSocket) {
                if(!connections[event.fd].outgoing.empty()) {
                    event.events = POLLIN | POLLOUT | POLLHUP;
                } else {
                    event.events = POLLIN | POLLHUP;
                }
            }
        }

    }
}

void Server::handleMessages() {
    for(auto& [fd, conn] : connections) {
        if(auto raw = conn.incoming.begin(); raw != conn.incoming.end()) {
            switch(raw->type) {

                // Login
                case MessageType::login:
                {
                    Message::login msg(*raw);
                    login(conn, msg.id);
                    break;
                }
                // Set name
                case MessageType::setName:
                {
                    if(conn.player) {
                        Message::setName msg(*raw);
                        conn.player->setName(msg.name);
                    }
                    break;
                }
                // Join a room
                case MessageType::joinRoom:
                {
                    if(conn.player) {
                        Message::joinRoom msg(*raw);
                        joinRoom(conn.player, msg.id);
                    }
                    break;
                }
                // Create a room
                case MessageType::createRoom:
                {
                    if(conn.player) {
                        Message::createRoom msg(*raw);
                        createRoom(conn.player, msg.settings);
                    }
                    break;
                }
                // Set a new host
                case MessageType::setNewHost:
                {
                    if(conn.player && conn.player->room) {
                        Message::setNewHost msg(*raw);
                        conn.player->room->setNewHost(*conn.player, msg.id);
                    }
                    break;
                }
                // Kick a player
                case MessageType::kickPlayer:
                {
                    if(conn.player && conn.player->room) {
                        Message::kickPlayer msg(*raw);
                        conn.player->room->kick(*conn.player, msg.id);
                    }
                    break;
                }
                // Leave a room
                case MessageType::leaveRoom:
                {
                    if(conn.player && conn.player->room) {
                        Room& room = *conn.player->room;
                        if(room.leave(conn.player)) {
                            // Empty room - delete
                            rooms.erase(room.id);
                        }
                    }
                    break;
                }
                // Start the game
                case MessageType::startGame:
                {
                    if(conn.player && conn.player->room) {
                        startGame(*conn.player->room);
                    }
                    break;
                }
                default:
                    printf("Got message of unimplemented type: %d\n", static_cast<uint8_t>(raw->type));
            }
            conn.incoming.erase(raw);
        }
    }
}

void Server::connect() {
    sockaddr_in socket;
    socklen_t socketSize;
    
    int fd = accept(newConnectionSocket, (sockaddr*)&socket, &socketSize);

    // Create new Connection
    connections.emplace(fd, fd);

    // Create new event
    events.push_back({ fd, POLLIN | POLLHUP });

    printf("Connected: %s, fd: %d, read_event: %lu\n",
           inet_ntoa(socket.sin_addr), fd, connections.size());

}

void Server::disconnect(int fd) {

    // Remove reference to Connection from Player
    if(connections[fd].player) {

        // Remove player from room
        if(connections[fd].player->room) {
            std::string roomID = connections[fd].player->room->id;
            if(connections[fd].player->room->leave(connections[fd].player)) {
                rooms.erase(roomID);
            }
        }

        // TODO: If player not in game, delete player
        
        connections[fd].player->conn = nullptr;
    }

    // Remove connection
    connections.erase(fd);

    // Remove event
    events.erase(std::remove_if(events.begin(), events.end(), [fd] (const pollfd& p) { return p.fd == fd; } ), events.end());

    printf("Disconnected: fd: %d\n", fd);
}


// MARK: - Handling messages

void Server::login(Connection& conn, std::optional<uint16_t> existingID) {

    // Login existing Player
    if(auto id = existingID) {

        // Find a player with the same ID (and no connection)
        auto it = players.find(*id);
        if(it != players.end()) {
            // Player ID found, assign Player to Connection and Connection to Player
            conn.player = &it->second;
            it->second.conn = &conn;

            // Send 'Logged in' message (containing player ID) and possible room settings
            conn.outgoing.push_back(Message::loggedIn(*id));
            conn.outgoing.push_back(Message::roomSettings(config.roomSettings));

            // TODO: Check if game in progress
            return;
        }
    }

    // Generate unique player ID
    uint16_t rID;
    std::uniform_int_distribution<uint16_t> dist{};
    do {
        rID = dist(generator);
    } while(players.find(rID) != players.end());

    printf("Random player id: %u\n", rID);

    // Create new Player and login
    auto it = players.emplace(rID, rID);
    conn.player = &it.first->second;
    it.first->second.conn = &conn;

    // Send 'Logged in' message (containing player ID) and possible room settings
    conn.outgoing.push_back(Message::loggedIn(rID));
    conn.outgoing.push_back(Message::roomSettings(config.roomSettings));
}

// Join a room
void Server::joinRoom(Player* player, std::string id) {
    if(auto it = rooms.find(id); it != rooms.end()) {
        it->second.join(player);
    } else {
        player->send(Message::error(MessageError::roomNotFound));
    }
}

// Create a room
void Server::createRoom(Player* player, RoomSettings& settings) {

    // Generate new room ID
    std::string id;
    std::uniform_int_distribution<char> dist('0', '9');

    // Create unique id
    do {
        id = {dist(generator), dist(generator), dist(generator), dist(generator), dist(generator), dist(generator)};
    } while (rooms.find(id) != rooms.end());

    printf("Generated room id: %s\n", id.c_str());

    // Create the new room and assign a pointer to it to the player
    auto [newRoom, _] = rooms.emplace(std::make_pair(id, Room(id, player, settings)));
    player->room = &newRoom->second;
    
}

// Start a game
void Server::startGame(Room& room) {
    auto game = games.insert(games.end(), room.start());
    game->setupPlayers();
    rooms.erase(room.id);
}


Server::~Server() {
    shutdown(newConnectionSocket, SHUT_RDWR);
    close(newConnectionSocket);
    
    
    /*
     wyczyscic wszystko
     - vector players
     - vector rooms
     
     */
}
