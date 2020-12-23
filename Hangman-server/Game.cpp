//
//  Game.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include "Game.hpp"

Game::Game(gameSettings settings, std::vector<Player*> players) {
    this->settings = settings;
    this->players = players;
}
