//
//  Room.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include "Room.hpp"

Room::Room(std::string id, Player* host, RoomSettings& settings) {
    this->id = id;
    this->settings = settings;
    this->players.insert(host);
    this->host = host;
    updateAll();
}

void Room::join(Player* player) {
    this->players.insert(player);
    player->room = this;
    updateAll();
}

bool Room::leave(Player* player) {
    players.erase(player);
    player->room = nullptr;

    if(players.empty()) {
        return true;
    } else {
        // If the host left the room, set a new one
        if(player == host) {
            host = *players.begin();
        }
        updateAll();
        return false;
    }
}

void Room::setNewHost(Player* currentHost, uint16_t newHostID) {
    if(currentHost == host) {
        auto it = std::find_if(players.begin(),
                               players.end(),
                               [newHostID] (const auto& p) { return p->id == newHostID; });
        if(it != players.end()) {
            host = *it;
            updateAll();
        }
    }
}

void Room::kick(Player* currentHost, uint16_t id) {
    if(currentHost == host) {
        auto it = std::find_if(players.begin(),
                               players.end(),
                               [id] (const auto& p) { return p->id == id; });
        if(it != players.end()) {
            players.erase(it);
            (*it)->room = nullptr;
            (*it)->send(Message::kicked());
            updateAll();
        }
    }
}

Game Room::start() {
    for(auto& player : players) {
        player->room = nullptr;
    }
    return Game(settings, players);
}

void Room::updateAll() {
    Message::Out msg = Message::roomStatus(settings, id, players, host);

    for(const auto& player : players) {
        player->send(msg);
    }
}

void Room::printPlayers() {
    for(const auto& player : players) {
        std::cout << player->getName() << std::endl;
    }
}
