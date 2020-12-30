//
//  Room.hpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#ifndef Room_hpp
#define Room_hpp

#include <vector>
#include <iostream>
#include "Player.hpp"
#include "Game.hpp"

struct PossibleRoomSettings {
    std::vector<std::string> languages;
    uint8_t wordLength[2];
    uint16_t gameTime[2];
    uint8_t healthPoints[2];
    uint8_t playerCount[2];
};

enum roomStatus {lobby, inGame, closed};

class Room {
public:
    Room(std::string roomId, Player* host);
    std::string getId();
    
    /// Add a player to the room
    void addPlayer(Player* player);
    
    void setLanguage(language lang);
    void setguessWordLength(int value);
    void setGameTime(int seconds);
    void setPlayerHealth(int value);
    void setMaxPlayers(int value);
    
    Game* game;
    
    void startGame();
    void updateGame(); // wyslij kazdemu graczowi aktualizacje?
    
    
    
    
    // DEBUG
    void printPlayers();

    /// Possible room settings
    // TODO: Read from config file
    static PossibleRoomSettings possibleSettings;
    
    
private:
    std::vector<Player*> players;
    std::string id;
    roomStatus status = lobby;
    gameSettings settings;
};

#endif /* Room_hpp */
