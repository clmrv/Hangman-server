//
//  Game.hpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#ifndef Game_hpp
#define Game_hpp

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
    Game(gameSettings settings);
    void start();
    
private:
    gameSettings settings;
};

#endif /* Game_hpp */
