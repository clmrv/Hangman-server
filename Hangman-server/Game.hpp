//
//  Game.hpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#ifndef Game_hpp
#define Game_hpp

#include "Room.hpp"

#include <set>
#include <map>
#include <string>
#include <chrono>

class Player;
struct RoomSettings;

struct PlayerInGame {
    const Player* player;
    uint16_t points;
    uint8_t health;
    std::u32string word;
    bool guessed;
};

class Game {
public:
    Game(RoomSettings& settings, std::set<Player*>& players);

    /// Assign a pointer to the game to each player
    void setupPlayers();

    void checkTime();

    void guessWord(std::u32string& word);

    void guessLetter(char32_t& letter);
    
private:
    /// Players
    std::map<Player*, PlayerInGame> players;

    /// Guessed word
    std::u32string word;

    /// Game start time
    std::chrono::time_point<std::chrono::system_clock> startTime;

    /// Game end time
    std::chrono::time_point<std::chrono::system_clock> endTime;

    /// Send a 'gameStatus' message to all players
    void updateAll();

    /// End the game and send 'scoreBoard' message to all players
    void endGame();

    /// Generate a random word
    /// @param language Word language
    /// @param length The length of the word
    static std::string randomWord(std::string language, uint8_t length);
};

#endif /* Game_hpp */
