//
//  Room.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include "Room.hpp"

PossibleRoomSettings Room::possibleSettings = {
    .languages = { "pl", "us" },
    .wordLength = { 3, 30 },
    .gameTime = { 30, 300 },
    .healthPoints = { 1, 5 },
    .playerCount = { 2, 5 }
};

Room::Room(std::string roomId, Player* host) {
    host->room = this;
    //host->setHost(true);
    
    this->id = roomId;
    this->players.push_back(host);
}


void Room::startGame() {
    this->game = new Game(settings, players);
}

void Room::addPlayer(Player* player) {
    this->players.push_back(player);
    player->room = this;
    // TODO: send message to all players
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
       // if ((*it)->isHost())
       //     std::cout << " host";
        std::cout << std::endl;
    }
}
