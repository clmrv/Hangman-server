//
//  Game.cpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#include "Game.hpp"

#include <locale>
#include <codecvt>

// Init random number generator
std::mt19937_64 Game::rng = std::mt19937_64 { (uint64_t)std::chrono::high_resolution_clock::now().time_since_epoch().count() };

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
    // LENGTH: 3-13
    std::ifstream file("./words/" + language + "/" + std::to_string(length));

    std::string word;

    // Load the number of words from the first line of the file
    std::getline(file, word);
    uint64_t words = std::stoll(word);

    // Random word number
    uint64_t rand = std::uniform_int_distribution<uint64_t>(0, words - 1)(Game::rng);

    for(uint64_t i = 0; i < rand; i++) {
        if(!std::getline(file, word)) {
            std::cout << "Error loading file\n";
            file.close();
            return std::string("X", length);
        }
    }
    file.close();

    std::cout << "Guess word: " << word << std::endl;
    return word;
}

void Game::updateAll() {

    // Get epoch end time
    auto epoch = std::chrono::duration_cast<std::chrono::seconds>(endTime.time_since_epoch()).count();

    Message::gameStatusBuilder builder(epoch, players, word.size());

    // Send message to every player
    for(const auto& [player, inGame] : players) {
        player->send( builder.setWord(inGame.word).build() );
    }
    
}
