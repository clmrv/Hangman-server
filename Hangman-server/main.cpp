//
//  main.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include <iostream>
#include "Server.hpp"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>

int main(int argc, const char * argv[]) {
    
    Server server;
    
    Player* player = new Player((int)server.players.size()); // id = index
    player->setName();
    server.players.push_back(player);
    
    int roomIndex = server.createRoom(server.players[0]);
    
    player = new Player((int)server.players.size());
    player->setName("xd");
    server.players.push_back(player);
    
    server.rooms[roomIndex]->addPlayer(player);
    
    std::cout << "index: " << roomIndex << std::endl;
    std::cout << "id: " << server.rooms[roomIndex]->getId() << std::endl;
    server.rooms[roomIndex]->printPlayers();
    
    server.rooms[roomIndex]->startGame();
    
    // ------------------------------------------
    
    int newConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (newConnectionSocket == -1) {
        printf("socked failed\n");
        return -1;
    }
    const int one = 1;
    setsockopt(newConnectionSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
   
    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(1234);
    servAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(newConnectionSocket, (sockaddr*)&servAddr, sizeof(servAddr) ) == -1) {
        printf("Binding failed: %d\n", errno);
        return -1;
    }

    listen(newConnectionSocket, 5);     // second arg - how many can wait to be accepted (in queue)
    
    const int eventCount = 1;
    pollfd events[eventCount];
    events[0].events = POLLIN;
    events[0].fd = newConnectionSocket;

    while (true) {
        if (poll(events, eventCount, -1) > 0) {
            if (events[0].revents & POLLIN) {
                // read from socket, write to stdout
                sockaddr_in newUserSocket;
                socklen_t newUserSocketSize;
                
                int newUserFD = accept(newConnectionSocket, (sockaddr*)&newUserSocket, &newUserSocketSize);
                write(newUserFD, "connected\n", 10);
            }
        }
    }
    


    shutdown(newConnectionSocket, SHUT_RDWR);
    close(newConnectionSocket);

    
    return 0;
}
