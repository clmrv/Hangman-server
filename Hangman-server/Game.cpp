//
//  Game.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include "Game.hpp"

#include <locale>
#include <codecvt>

Game::Game(RoomSettings& settings, std::set<Player*>& players) {

    // Get UTF-8 word
    std::string utf8word = Game::randomWord(settings.language, settings.wordLength);
    // Convert to UTF-32 and assing to `word`
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    this->word = conv.from_bytes(utf8word);

    // Start and end time of the game
    this->startTime = std::chrono::system_clock::now();
    this->endTime = this->startTime + std::chrono::seconds(settings.gameTime);

    // Make word with no guesses
    std::u32string emptyWord(settings.wordLength, '\0');

    // Insert all players and set game
    for(const auto& player : players) {
        this->players.insert(std::make_pair(player, PlayerInGame {
            player,                 // Player*
            0,                      // Points
            settings.healthPoints,  // Health
            emptyWord,              // Empty word
            false                   // Guessed word
        }));
    }
}

void Game::setupPlayers() {
    for(auto& [player, inGame] : players) {
        player->game = this;
    }
}

std::string Game::randomWord(std::string language, uint8_t length) {
    return "Wąż";
}

void Game::updateAll() {

    // Calculate remaining time
    auto diff = endTime - std::chrono::system_clock::now();
    uint16_t remainingTime = std::chrono::duration_cast<std::chrono::seconds>(diff).count();

    Message::gameStatusBuilder builder(remainingTime, players, word.size());

    // Send message to every player
    for(const auto& [player, inGame] : players) {
        player->send( builder.setWord(inGame.word).build() );
    }
    
}
