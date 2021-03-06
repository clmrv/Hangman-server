//
//  Game.hpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 22/12/2020.
//

#ifndef Game_hpp
#define Game_hpp

#include "Room.hpp"
#include "plog/Log.h"

#include <set>
#include <map>
#include <string>
#include <chrono>
#include <fstream>
#include <random>
#include <algorithm>

class Player;
struct RoomSettings;

struct PlayerInGame {
    const Player* player;
    uint16_t points;
    uint8_t health;
    std::u32string word;
    bool guessed;

    bool operator<(const PlayerInGame& other) const;
    bool operator>(const PlayerInGame& other) const;
};

// Constants used for calculating points
namespace GamePoints {

constexpr uint16_t wordConstant = 1000;
constexpr double wordTime = 2000;
constexpr double wordLetters = 100;
constexpr double wordHealth = 1000;

constexpr uint16_t letter = 20;
constexpr uint16_t letterBonus = 10;

}

class Game {
public:
    Game(const std::string& id, RoomSettings& settings, std::set<Player*>& players);

    /// 6-digit ID of the game
    std::string id;

    /// Assign a pointer to the game to each player
    void setupPlayers();

    /// Update game status
    /// @return Whether the game is finished and can be deleted
    bool loop();

    /// Send a 'gameStatus' message to a returning player
    void playerReturned(Player* player);

    /// Try to guess the word
    /// @param player Player guessing the word
    /// @param word The guessed word
    /// @return Whether the game is finished and can be deleted (eg. all players guessed the word or lost health)
    bool guessWord(Player* player, std::u32string& word);

    /// Try to guess a letter
    /// @param player Player guessing the letter
    /// @param letter The guessed letter
    bool guessLetter(Player* player, char32_t& letter);

    /// Get game end time
    std::chrono::time_point<std::chrono::system_clock> getEndTime() const;
    
private:
    /// Players
    std::map<Player*, PlayerInGame> players;

    /// Guessed word
    std::u32string word;

    /// Max player health
    uint8_t maxHealth;

    /// Game start time
    std::chrono::time_point<std::chrono::system_clock> startTime;

    /// Game end time
    std::chrono::time_point<std::chrono::system_clock> endTime;

    /// Delete a pointer to the game from each player
    void teardownPlayers();

    /// Send a 'gameStatus' message to all players
    void sendGameStatus();

    /// Send a 'scoreBoard' message to all players
    void sendScoreboard();

    /// End the game and send 'scoreBoard' message to all players
    void endGame();

    /// Generate a random word
    /// @param language Word language
    /// @param length The length of the word
    static std::string randomWord(const std::string& language, uint8_t length);

    /// Random number generator
    static std::mt19937_64 rng;
};

#endif /* Game_hpp */
