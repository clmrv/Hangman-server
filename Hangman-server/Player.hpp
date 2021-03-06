//
//  Player.hpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 22/12/2020.
//


#ifndef Player_hpp
#define Player_hpp

#include <string>
#include "Connection/Connection.hpp"
#include "Connection/OutMessage.hpp"
#include "plog/Log.h"

class Room;
class Game;
class Connection;

class Player {
public:

    /// Create a new player
    /// @param id Player identifier
    Player(uint16_t id);

    // Player identifier
    uint16_t id;

    // Connection used by this Player
    Connection* conn = nullptr;

    /// Send a message to client (if there is a connection)
    void send(Message::Out message);

    // Set the name of the Player
    void setName(std::string name = "");

    /// Get the name of the Player
    std::string getName() const;

    Room* room = nullptr;

    Game* game = nullptr;

private:
    /// Player nickname
    std::string name;

};

#endif /* Player_hpp */
