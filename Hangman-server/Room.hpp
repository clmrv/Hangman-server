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

enum roomStatus {lobby, inGame, closed};

class Room {
public:
    Room(std::string roomId, Player* host);
    std::string getId();
    
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
    
    
private:
    std::vector<Player*> players;
    std::string id;
    roomStatus status = lobby;
    gameSettings settings;
};

#endif /* Room_hpp */
