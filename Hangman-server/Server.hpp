//
//  Server.hpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#ifndef Server_hpp
#define Server_hpp

#include "Room.hpp"
#include "Player.hpp"
#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Server {
public:
    
    Server(int port);
    /// returns index of room
    int createRoom(Player* host);
    
    ~Server(); 
    
    
private:
    std::vector<Player*> players;
    std::vector<Room*> rooms;
    
    pollfd events[500];
    int currFreeEventIndex = 0;
    
    int newConnectionSocket;
    
    void eventLoop();
    std::string generateRoomId();
    void connectPlayer();
};

#endif /* Server_hpp */
