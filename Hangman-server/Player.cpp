//
//  Player.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include "Player.hpp"

Player::Player( int fd, int readEvent) {
    this->fd = fd;
    this->readEvent = readEvent;
}


void Player::setName(std::string name) {
    if (name.empty())
        this->name = generateName();
    else
        this->name = name;
}

std::string Player::generateName() {
    std::string name;
    name = "User" + std::to_string(fd);
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
