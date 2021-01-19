//
//  Room.hpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 22/12/2020.
//

#ifndef Room_hpp
#define Room_hpp

#include <set>
#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "Player.hpp"
#include "Game.hpp"
#include "plog/Log.h"

class Game;

struct PossibleRoomSettings {
    std::vector<std::string> languages;
    std::array<uint8_t, 2> wordLength;
    std::array<uint16_t, 2> gameTime;
    std::array<uint8_t, 2> healthPoints;
    std::array<uint8_t, 2> playerCount;
};

struct RoomSettings {
    std::string language;
    uint8_t wordLength = 6;
    uint16_t gameTime = 300; //seconds
    uint8_t healthPoints = 5;
    uint8_t maxPlayers = 4;
};

class Room {
public:

    /// 6-digit ID of the room
    std::string id;

    /// Create a new room
    /// @param id ID of the room
    /// @param host Player creating the room
    /// @param settings Settings of the room
    Room(std::string id, Player* host, RoomSettings& settings);


    /// Whether the player can join the room
    bool canJoin();
    
    /// Join the room
    void join(Player* player);

    /// Leave the room
    /// @return `true` if this room is now empty and should be deleted
    bool leave(Player* player);

    /// Set new host of the room
    /// @param currentHost The host requesting a change
    /// @param newHostID ID of the new host
    void setNewHost(Player* currentHost, uint16_t newHostID);

    /// Kick a player from the room
    /// @param currentHost The host requesting a kick
    /// @param id ID of the player to be kicked
    void kick(Player* currentHost, uint16_t id);

    /// Starts a game
    /// @note The room can now be deleted
    Game start();
    
private:
    /// Players currently in the room
    std::set<Player*> players;

    /// Player who is the host of the room
    Player* host = nullptr;

    /// Settings of this room
    RoomSettings settings;

    /// Send a 'gameStatus' message to all players
    void updateAll();
};

#endif /* Room_hpp */
