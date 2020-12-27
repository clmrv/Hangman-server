//
//  Server.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include "Server.hpp"

Server::Server(int port) {
    newConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (newConnectionSocket == -1) {
        printf("socked failed\n");
        return;
    }
    const int one = 1;
    setsockopt(newConnectionSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
   
    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
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

        if(poll(events.data(), (unsigned int)events.size(), -1) > 0) {
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

                    // Disconnect
                    if(event.revents & POLLHUP) {
                        disconnect(event.fd);
                    }

                }

                // Login players
                // (should be done on separate thread?)
                for(auto& [fd, conn] : connections) {

                    // If the first message is of type 'LOGIN'
                    if(!conn.incoming.empty() > 0 && conn.incoming[0].type == 'X') {

                        // Message contains restoration ID
                        if(conn.incoming[0].length == 4) {
                            uint16_t restorationID;
                            memcpy(&restorationID, conn.incoming[0].data, sizeof(uint16_t));
                            restorationID = ntohs(restorationID);
                            loginPlayer(conn, restorationID);
                        }
                        // Message without restoration ID
                        else {
                            loginPlayer(conn);
                        }
                    }
                }


            }
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
    if(connections[fd].player != nullptr) {
        connections[fd].player->conn = nullptr;
    }

    // Remove connection
    connections.erase(fd);

    // Remove event
    events.erase(std::remove_if(events.begin(), events.end(), [fd] (const pollfd& p) { return p.fd == fd; } ), events.end());

    printf("Disconnected: fd: %d\n", fd);
}


int Server::createRoom(Player* host) {
    auto it = rooms.emplace_back(generateRoomId(), host);
    return (int)rooms.size()-1;
}

std::string Server::generateRoomId() {
    std::string id;
    
    // TODO
    // nr pokoju + 4 litery?
    
    // albo losujemy 4 litery, potem sprawdzamy, czy się w jakimś pokoju powtarza, jak tak to losujemy ponownie
    
    return "test";
}

void Server::loginPlayer(Connection& conn, std::optional<uint16_t> restorationID) {

    // Login existing Player
    if(auto id = restorationID) {

        // Find a player with the same restoration ID (and no connection)
        auto it = std::find_if(players.begin(), players.end(), [id] (const Player& p) {
            return p.restorationId == id && p.conn == nullptr;
        });

        // Restoration ID found, assign Player to Connection and Connection to Player
        if(it != players.end()) {
            conn.player = &(*it);
            it->conn = &conn;
            return;
        }
    }

    // No restoration ID
    // Create new Player and login
    auto it = players.emplace_back();
    conn.player = &it;
    it.conn = &conn;
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
