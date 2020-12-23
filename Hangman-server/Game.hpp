//
//  Game.hpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#ifndef Game_hpp
#define Game_hpp

#include <vector>

class Player;

enum language {Polski, English};

struct gameSettings
{
    language lang = Polski;
    int guessWordLength = 6;
    int gameTime = 300; //seconds
    int health = 5;
    int maxPlayers = 4;
};

class Game {
public:
    Game(gameSettings settings, std::vector<Player*> players);
    
private:
    gameSettings settings;
    std::vector<Player*> players;
};

#endif /* Game_hpp */
