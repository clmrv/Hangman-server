//
//  Room.cpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 22/12/2020.
//

#include "Room.hpp"

Room::Room(std::string id, Player* host, RoomSettings& settings) {
    this->id = id;
    this->settings = settings;
    this->players.insert(host);
    this->host = host;

    PLOGI << "Creating a room with id " << id << ", host: #" << host->id << " - " << host->getName();
    IF_PLOG(plog::verbose) {
        PLOGV << "Room settings:";
        PLOGV << "\tlang: " << settings.language;
        PLOGV << "\tword length: " << (int)settings.wordLength;
        PLOGV << "\tgame time: " << (int)settings.gameTime << " seconds";
        PLOGV << "\thealth points: " << (int)settings.healthPoints;
        PLOGV << "\tmax players: " << (int)settings.maxPlayers;
    }

    updateAll();
}

void Room::join(Player* player) {
    this->players.insert(player);
    player->room = this;
    PLOGI << "Player #" << player->id << " - " << player->getName() << " joined the room " << id;
    updateAll();
}

bool Room::leave(Player* player) {
    players.erase(player);
    player->room = nullptr;

    PLOGI << "Player #" << player->id << " - " << player->getName() << " left the room " << id;

    if(players.empty()) {
        PLOGD << "Empty room " << id << ", can be deleted";
        return true;
    } else {
        // If the host left the room, set a new one
        if(player == host) {
            host = *players.begin();
            PLOGI << "Room " << id << ": Host left, new host: #" << host->id << " - " << host->getName();
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
            PLOGI << "Player #" << currentHost->id << " - " << currentHost->getName() << " changing host to #"
            << (*it)->id << " - " << (*it)->getName();

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
            PLOGI << "Player #" << currentHost->id << " - " << currentHost->getName() << " kicked player #"
            << (*it)->id << " - " << (*it)->getName();

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
    PLOGI << "Room " << id << " starting game";
    return Game(settings, players);
}

void Room::updateAll() {
    Message::Out msg = Message::roomStatus(settings, id, players, host);

    for(const auto& player : players) {
        player->send(msg);
    }
}
