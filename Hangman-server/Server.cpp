//
//  Server.cpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 22/12/2020.
//

#include "Server.hpp"
#include "Connection/Message.hpp"

Server::Server(): config("config") {
    PLOGI << "Served initialized";
}

void Server::start() {
    newConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (newConnectionSocket == -1) {
        PLOGE << "Socket failed";
        return;
    }
    const int one = 1;
    setsockopt(newConnectionSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
   
    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(config.port);
    servAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(newConnectionSocket, (sockaddr*)&servAddr, sizeof(servAddr) ) == -1) {
        PLOGE << "Binding failed: " << errno;
        return;
    }

    listen(newConnectionSocket, 5);     // second arg - how many can wait to be accepted (in queue)
    PLOGD << "Server listening on port " << config.port;
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

                    // Disconnect
                    if(event.revents & POLLHUP) {
                        disconnect(event.fd);
                    }

                    // Read from connection
                    if(event.revents & POLLIN) {
                        connections[event.fd].read();
                    }

                    // Write to connection
                    if(event.revents & POLLOUT) {
                        connections[event.fd].write();
                    }


                }
            }

            handleMessages();
        }

        handleGames();

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

            PLOGD << "Connection " << fd << " received message of type 0x" << std::hex << (int)static_cast<uint8_t>(raw->type);

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
                    } else {
                        PLOGW << "No player, but received 'setName' message";
                    }
                    break;
                }
                // Join a room
                case MessageType::joinRoom:
                {
                    if(conn.player) {
                        Message::joinRoom msg(*raw);
                        joinRoom(conn.player, msg.id);
                    } else {
                        PLOGW << "No player, but received 'joinRoom' message";
                    }
                    break;
                }
                // Create a room
                case MessageType::createRoom:
                {
                    if(conn.player) {
                        Message::createRoom msg(*raw);
                        createRoom(conn.player, msg.settings);
                    } else {
                        PLOGW << "No player, but received 'createRoom' message";
                    }
                    break;
                }
                // Set a new host
                case MessageType::setNewHost:
                {
                    if(conn.player && conn.player->room) {
                        Message::setNewHost msg(*raw);
                        conn.player->room->setNewHost(conn.player, msg.id);
                    } else {
                        PLOGW << "No player, but received 'setNewHost' message";
                    }
                    break;
                }
                // Kick a player
                case MessageType::kickPlayer:
                {
                    if(conn.player && conn.player->room) {
                        Message::kickPlayer msg(*raw);
                        conn.player->room->kick(conn.player, msg.id);
                    } else {
                        PLOGW << "No player or player not in room, but received 'kickPlayer' message";
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
                    } else {
                        PLOGW << "No player or player not in room, but received 'leaveRoom' message";
                    }
                    break;
                }
                // Start the game
                case MessageType::startGame:
                {
                    if(conn.player && conn.player->room) {
                        startGame(*conn.player->room);
                    } else {
                        PLOGW << "No player or player not in room, but received 'startGame' message";
                    }
                    break;
                }
                // Guess a word
                case MessageType::guessWord:
                {
                    if(conn.player && conn.player->game) {
                        Message::guessWord msg(*raw);
                        Game& game = *conn.player->game;
                        if(game.guessWord(conn.player, msg.word)) {
                            // Game finished - can delete
                            PLOGD << "Game finished - deleting";
                            games.erase(game.id);
                        }
                    } else {
                        PLOGW << "No player or player not in game, but received 'guessWord' message";
                    }
                    break;
                }
                // Guess a letter
                case MessageType::guessLetter:
                {
                    if(conn.player && conn.player->game) {
                        Message::guessLetter msg(*raw);
                        Game& game = *conn.player->game;
                        if(game.guessLetter(conn.player, msg.letter)) {
                            // Game finished - can delete
                            PLOGD << "Game finished - deleting";
                            games.erase(game.id);
                        }
                    } else {
                        PLOGW << "No player or player not in room, but received 'guessLetter' message";
                    }
                    break;
                }
                default:
                    PLOGW << "Message of unknown type: " << std::hex << (int)static_cast<uint8_t>(raw->type);
                    break;
            }
            conn.incoming.erase(raw);
        }
    }
}

void Server::handleGames() {
    // Loop every game
    for(auto it = games.begin(); it != games.end(); ) {
        if(it->second.loop()) {
            // Delete game when finished
            PLOGD << "Game finished - deleting";
            it = games.erase(it);
        } else {
            it++;
        }
    }
}

void Server::connect() {
    sockaddr_in socket;
    socklen_t socketSize;
    
    int fd = accept(newConnectionSocket, (sockaddr*)&socket, &socketSize);

    // Set file descriptor to non-blocking
    if (fcntl(fd, F_SETFL, O_NONBLOCK, 1) == -1) {
        close(fd);
        PLOGE << "Error setting file descriptor " << fd << " to non-blocking";
        return;
    }

    // Create new Connection
    connections.emplace(fd, fd);

    // Create new event
    events.push_back({ fd, POLLIN | POLLHUP });

    PLOGI << "Connected IP: " << inet_ntoa(socket.sin_addr) << ", FD: " << fd << ", connections: " << connections.size();
}

void Server::disconnect(int fd) {

    // Remove reference to Connection from Player
    if(connections[fd].player) {
        PLOGI << "Disconnect player #" << connections[fd].player->id
              << ", NAME: " << connections[fd].player->getName() << ", FD: " << fd;

        // Remove player from room
        if(connections[fd].player->room) {
            PLOGD << "Removing player #" << connections[fd].player->id << " from room " << connections[fd].player->room->id;
            std::string roomID = connections[fd].player->room->id;
            if(connections[fd].player->room->leave(connections[fd].player)) {
                PLOGD << "Deleting room " << roomID << " - was empty";
                rooms.erase(roomID);
            }
        }

        auto playerIt = players.find(connections[fd].player->id);

        connections[fd].player->conn = nullptr;

        // If player not in game, delete player
        if(!playerIt->second.game) {
            PLOGD << "Deleting player #" << playerIt->second.id;
            players.erase(playerIt);
        }
        

    } else {
        PLOGI << "Disconnect player FD: " << fd;
    }

    // Remove connection
    connections.erase(fd);

    // Remove event
    events.erase(std::remove_if(events.begin(), events.end(), [fd] (const pollfd& p) { return p.fd == fd; } ), events.end());

    // Close fd
    close(fd);

    PLOGV << "Deleted player FD: " << fd;
}


// MARK: - Handling messages

void Server::login(Connection& conn, std::optional<uint16_t> existingID) {

    // Login existing Player
    if(auto id = existingID) {
        PLOGD << "Login request from FD: " << conn.fd << " to player #" << *id;

        // Find a player with the same ID (and no connection)
        if(auto it = players.find(*id); it != players.end() && it->second.conn == nullptr) {
            PLOGI << "Login player #" << it->second.id << " to connection with FD: " << conn.fd;
            // Player ID found, assign Player to Connection and Connection to Player
            conn.player = &it->second;
            it->second.conn = &conn;

            // Send 'Logged in' message (containing player ID) and possible room settings
            conn.outgoing.push_back(Message::loggedIn(*id));
            conn.outgoing.push_back(Message::roomSettings(config.roomSettings));

            // Send a message if the player returned
            if(it->second.game) {
                PLOGD << "Player #" << it->second.id << " is in game";
                it->second.game->playerReturned(&it->second);
            }
            return;
        } else {
            PLOGD << "Player with #" << *id << " not found or already has a connection";
        }
    }
    PLOGD << "Login request from FD: " << conn.fd << " - without id";

    // Generate unique player ID
    uint16_t rID;
    std::uniform_int_distribution<uint16_t> dist{};
    do {
        rID = dist(generator);
    } while(players.find(rID) != players.end());

    PLOGI << "Logged in player with random ID: " << rID;

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
        if(it->second.canJoin()) {
            PLOGI << "Player #" << player->id << " joined room " << it->second.id;
            it->second.join(player);
        } else {
            PLOGI << "Player #" << player->id << " tried to join full room " << it->second.id;
            player->send(Message::error(MessageError::roomFull));
        }
    } else {
        PLOGI << "Player #" << player->id << " tried to join not-existing room " << id;
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
    } while (rooms.find(id) != rooms.end() || games.find(id) != games.end());

    // Create the new room and assign a pointer to it to the player
    auto [newRoom, _] = rooms.emplace(id, Room(id, player, settings));
    player->room = &newRoom->second;

    PLOGI << "Player #" << player->id << " created room " << id;
}

// Start a game
void Server::startGame(Room& room) {
    auto [newGame, _] = games.emplace(room.id, room.start());
    newGame->second.setupPlayers();
    PLOGI << "Game from room " << room.id << " started.";
    rooms.erase(room.id);
}


Server::~Server() {
    shutdown(newConnectionSocket, SHUT_RDWR);
    close(newConnectionSocket);
    PLOGD << "Shutdown server";
}
