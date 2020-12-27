//
//  Connection.hpp
//  Hangman-server
//
//  Created by Przemek Ambroży on 27/12/2020.
//

#ifndef Connection_hpp
#define Connection_hpp

#include "Player.hpp"
#include "Message.hpp"

#include <vector>

class Player;

class Connection {
private:
    InMessage nextIn;

public:
    int fd;
    Player* player;
    std::vector<InMessage> incoming;


    Connection();
    Connection(int fd);

    /// Read messages
    void read();

};


#endif /* Connection_hpp */
