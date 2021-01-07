//
//  main.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include <iostream>
#include "plog/Log.h"
#include "plog/Initializers/RollingFileInitializer.h"
#include "plog/Appenders/RollingFileAppender.h"
#include "plog/Appenders/ConsoleAppender.h"
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

    static plog::RollingFileAppender<plog::TxtFormatterEmoji> fileAppender("data.log", 1000000, 1);
    static plog::ConsoleAppender<plog::TxtFormatterEmoji> consoleAppender;
    plog::init(plog::debug, &fileAppender).addAppender(&consoleAppender);

    Server server(1234);

    return 0;
}
