//
//  main.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include <iostream>
#include "Server.hpp"
#include "Connection/Connection.hpp"


int main(int argc, const char * argv[]) {
    /*
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
    std::cout << "id: " << server.rooms[roomIndex].getId() << std::endl;
    server.rooms[roomIndex].printPlayers();
    
    server.rooms[roomIndex].startGame();
    */

    Server server(1234);

    return 0;
}
