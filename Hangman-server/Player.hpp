//
//  Player.hpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//


#ifndef Player_hpp
#define Player_hpp

#include <string>

/* możliwe, że można jakoś lepiej headery zrobić, żeby tego nie było trzeba*/
class Room;
/* ---------------------------------------------------------------------- */

class Player {
public:
    Player(int id /*pewnie jakies sieciowe info*/);
    
    void setName(std::string name = "");
    bool isHost();
    void setHost(bool value);
    
    Room* room;
    
    std::string getName();
    
    
private:
    int id;
    std::string name;
    int health;
    bool host = false;
    
    std::string generateName();
    
};

#endif /* Player_hpp */
