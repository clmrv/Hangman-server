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

#include <list>

class Player;

class Connection {
private:
    Message::In nextIn;

public:
    int fd;
    Player* player;
    std::list<Message::In> incoming;
    std::list<Message::Out> outgoing;


    Connection();
    Connection(int fd);

    /// Read messages
    void read();

    /// Write messages
    void write();

};


#endif /* Connection_hpp */
