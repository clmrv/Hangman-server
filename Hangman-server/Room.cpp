//
//  Room.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include "Room.hpp"

Room::Room(std::string roomId, Player* host) {
    host->room = this;
    host->setHost(true);
    
    this->id = roomId;
    this->players.push_back(host);
}


void Room::addPlayer(Player* player) {
    this->players.push_back(player);
}

void Room::setLanguage(language lang) {
    settings.lang = lang;
}

std::string Room::getId() {
    return this->id;
}


void Room::printPlayers() {
    for( std::vector<Player*>::iterator it = players.begin(); it != players.end(); it++) {
        std::cout << (*it)->getName();
        if ((*it)->isHost())
            std::cout << " host";
        std::cout << std::endl;
    }
}
