//
//  Player.hpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//


#ifndef Player_hpp
#define Player_hpp

#include <string>
#include "Connection.hpp"

class Room;
class Connection;

class Player {
public:
    Player(uint32_t restorationId);

    // Identifier used to connect back to the Player
    uint16_t restorationId;

    // Connection used by this Player
    Connection* conn;
    
    void setName(std::string name = "");
    bool isHost();
    void setHost(bool value);
    
    Room* room;
    
    std::string getName();
    
    
private:

    std::string name;
    int health;
    bool host = false;
    
    std::string generateName();
    
};

#endif /* Player_hpp */
