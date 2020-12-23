//
//  Server.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include "Server.hpp"

int Server::createRoom(Player* host) {
    Room* room = new Room(generateRoomId(), host);
    this->rooms.push_back(room);
    return (int)rooms.size()-1;
}

std::string Server::generateRoomId() {
    std::string id;
    
    // TODO
    // nr pokoju + 4 litery?
    
    // albo losujemy 4 litery, potem sprawdzamy, czy się w jakimś pokoju powtarza, jak tak to losujemy ponownie
    
    return "test";
}


Server::~Server() {
    /*
     wyczyscic wszystko
     - vector players
     - vector rooms
     
     */
}
