//
//  Player.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include "Player.hpp"

Player::Player(uint32_t restorationId) {
    this->restorationId = restorationId;
}


void Player::setName(std::string name) {
    if (name.empty())
        this->name = generateName();
    else
        this->name = name;
}

std::string Player::generateName() {
    std::string name;
    // TODO: Create random name
    name = "User000";
    return name;
}

bool Player::isHost() {
    return this->host;
}

void Player::setHost(bool value) {
    this->host = value;
}


// DEBUG

std::string Player::getName() {
    return this->name;
}
