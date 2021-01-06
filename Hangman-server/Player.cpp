//
//  Player.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include "Player.hpp"

Player::Player(uint16_t id) {
    this->id = id;
    // Temporary player name
    this->name = "Player" + std::to_string(id);
    this->room = nullptr;
    this->game = nullptr;
}

void Player::send(Message::Out message) {
    if(conn) {
        conn->outgoing.push_back(std::move(message));
    }
}

void Player::setName(std::string name) {
    if(!name.empty())
        this->name = name;
}


// DEBUG

std::string Player::getName() const {
    return this->name;
}
