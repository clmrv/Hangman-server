//
//  Player.cpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 22/12/2020.
//

#include "Player.hpp"

Player::Player(uint16_t id) {
    this->id = id;
    // Temporary player name
    this->name = "Player" + std::to_string(id);
    this->room = nullptr;
    this->game = nullptr;
    PLOGI << "Creating player #" << id << " - " << this->name;
}

void Player::send(Message::Out message) {
    if(conn) {
        conn->outgoing.push_back(std::move(message));
    }
}

void Player::setName(std::string name) {
    if(!name.empty()) {
        PLOGI << "Changing name of player #" << id << " from '" << this->name << "' to '" << name << "'";
        this->name = name;
    }
}

std::string Player::getName() const {
    return this->name;
}
