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

class Server {
public:
    std::vector<Room*> rooms;
    std::vector<Player*> players;
    
    /// returns index of room
    int createRoom(Player* host);
    
    ~Server(); 
    
    
private:
    std::string generateRoomId();
};

#endif /* Server_hpp */
