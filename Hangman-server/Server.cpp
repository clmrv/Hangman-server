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
    events[0].events = POLLIN;
    events[0].fd = newConnectionSocket;
    currFreeEventIndex++;

    while (true) {
        if (poll(events, currFreeEventIndex, -1) > 0) {
            
            // ACCEPT NEW CONNECTION
            if (events[0].revents & POLLIN) {
                connectPlayer();
            }
            
            
            // READ FROM PLAYERS
            for( std::vector<Player*>::iterator it = players.begin(); it != players.end(); it++) {
                if (events[(*it)->readEvent].revents & POLLIN) {
                    char buffer[256];
                    int bytes = read((*it)->fd, buffer, 256);
                    if (bytes > 0)
                        write(1, buffer, bytes);
                }
            }
            
        }
    }
}

void Server::connectPlayer() {
    sockaddr_in newUserSocket;
    socklen_t newUserSocketSize;
    
    int newUserFD = accept(newConnectionSocket, (sockaddr*)&newUserSocket, &newUserSocketSize);
    this->players.push_back(new Player(newUserFD, currFreeEventIndex));
    events[currFreeEventIndex].fd = newUserFD;
    events[currFreeEventIndex].events = POLLIN;
    
    printf("Connected: %s, fd: %d, read_event: %d\n", inet_ntoa(newUserSocket.sin_addr), newUserFD, currFreeEventIndex);
    
    currFreeEventIndex++;
}


int Server::createRoom(Player* host) {
    Room* room = new Room(generateRoomId(), host);
    this->rooms.push_back(room);
    return (int)rooms.size()-1;
}

std::string Server::generateRoomId() {
    std::string id;
    
    // TODO
    // nr pokoju + 4 litery?
    
    // albo losujemy 4 litery, potem sprawdzamy, czy się w jakimś pokoju powtarza, jak tak to losujemy ponownie
    
    return "test";
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
