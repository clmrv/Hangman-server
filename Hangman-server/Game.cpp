//
//  Game.cpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 22/12/2020.
//

#include "Game.hpp"

#include <locale>
#include <codecvt>

bool PlayerInGame::operator<(const PlayerInGame& other) const {
    return points < other.points;
}
bool PlayerInGame::operator>(const PlayerInGame& other) const {
    return points > other.points;
}

// Init random number generator
std::mt19937_64 Game::rng = std::mt19937_64 { (uint64_t)std::chrono::high_resolution_clock::now().time_since_epoch().count() };

Game::Game(RoomSettings& settings, std::set<Player*>& players) {

    // Get UTF-8 word
    std::string utf8word = Game::randomWord(settings.language, settings.wordLength);
    // Convert to UTF-32 and assing to `word`
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    this->word = conv.from_bytes(utf8word);

    PLOGI << "Creating game with word '" << utf8word << "'";

    // Remember player maximum health
    this->maxHealth = settings.healthPoints;

    // Start and end time of the game
    this->startTime = std::chrono::system_clock::now();
    this->endTime = this->startTime + std::chrono::seconds(settings.gameTime);

    // Make word with no guesses
    std::u32string emptyWord(settings.wordLength, '\0');

    PLOGV << "Players (" << players.size() << "):";
    // Insert all players and set game
    for(const auto& player : players) {
        this->players.insert(std::make_pair(player, PlayerInGame {
            player,                 // Player*
            0,                      // Points
            settings.healthPoints,  // Health
            emptyWord,              // Empty word
            false                   // Guessed word
        }));
        PLOGV << "\t#" << player->id << " - " << player->getName();
    }
}

bool Game::operator==(const Game &other) {
    bool basic = word == other.word && startTime == other.startTime && endTime == other.endTime && players.size() == other.players.size();
    if(!basic) {
        return false;
    }
    for(const auto& [player, inGame] : players) {
        if(other.players.find(player) == other.players.end()) {
            return false;
        }
    }
    return true;
}

void Game::setupPlayers() {
    PLOGV << "Setting up players";
    for(auto& [player, inGame] : players) {
        player->game = this;
    }
    sendGameStatus();
}

void Game::teardownPlayers() {
    PLOGV << "Setting no game to all players";
    for(auto& [player, inGame] : players) {
        player->game = nullptr;
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
            PLOGE << "Error loading file for language '" << language << "' with the length of " << length;
            file.close();
            return std::string("X", length);
        }
    }
    file.close();

    PLOGV << "Random word in language '" << language << "' with the length of " << length << " is: " << word;
    return word;
}

void Game::sendGameStatus() {

    PLOGV << "Sending game status to all players";

    // Get epoch end time
    auto epoch = std::chrono::duration_cast<std::chrono::seconds>(endTime.time_since_epoch()).count();

    Message::gameStatusBuilder builder(epoch, players, word.size());

    // Send message to every player
    for(const auto& [player, inGame] : players) {
        player->send( builder.setWord(inGame.word).build() );
    }
    
}

void Game::sendScoreboard() {

    PLOGV << "Sending scoreboard to all players";

    std::vector<PlayerInGame> sorted;
    sorted.reserve(players.size());

    for(const auto& [player, inGame] : players) {
        sorted.push_back(inGame);
    }

    std::sort(sorted.begin(), sorted.end(), std::greater<>());

    // Create 'scoreboard' message
    Message::Out msg = Message::scoreboard(sorted, word);

    // Send message to every player
    for(const auto& [player, inGame] : players) {
        Message::Out copied = msg;
        player->send(copied);
    }

}

bool Game::loop() {

    auto now = std::chrono::system_clock::now();
    auto remaining = std::chrono::duration_cast<std::chrono::seconds>(endTime - now).count();

    // If time is up, finish the game
    if(remaining <= 0) {
        PLOGD << "Time is up, finishing the game";
        sendScoreboard();
        teardownPlayers();
        return true;
    }

    // There is still some time left

    // True when all the players guessed or run out of health
    bool finished = std::find_if(players.begin(),
                                 players.end(),
                                 [] (const auto& p) {
                                     return !p.second.guessed || p.second.health > 0;
                                 }) == players.end();

    if(finished) {
        PLOGD << "All players finished, finishing the game";
        sendScoreboard();
        teardownPlayers();
    }

    return finished;
}

void Game::playerReturned(Player* player) {

    PLOGD << "Player #" << player->id << " returned to the game";

    // Get epoch end time
    auto epoch = std::chrono::duration_cast<std::chrono::seconds>(endTime.time_since_epoch()).count();

    player->send(
                 Message::gameStatusBuilder(epoch, players, word.size()).setWord(players[player].word).build()
                 );

}


bool Game::guessWord(Player *player, std::u32string &word) {
    PlayerInGame& p = players[player];

    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    std::string utf8word = conv.to_bytes(word);
    PLOGI << "Player #" << player->id << " is trying to guess the word '" << utf8word << "'";

    // If player hasn't guessed yet and has health left
    if(!p.guessed && p.health > 0) {

        // Player correctly guessed the word
        if(word == this->word) {
            p.guessed = true;
            p.word = word;
            PLOGI << "Player #" << player->id << " correctly guessed the word '" << utf8word << "'";

            auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
            auto remaining = std::chrono::duration_cast<std::chrono::seconds>(endTime - std::chrono::system_clock::now()).count();

            // 1.0 - player guessed the word at the beginning of the game
            // 0.0 - player guessed the word at the end of the game
            double timePercentage = (double)remaining / (double)duration;

            // Letters that were missing (not guessed)
            uint64_t missingLetters = std::count(p.word.begin(), p.word.end(), 0x00000000);
            // 1.0 - all letters were missing
            // 0.0 - none letters were missing
            double lettersPercentage = (double)missingLetters / (double)p.word.size();

            PLOGD << "Player #" << player->id << ": points: " << p.points << ", time%: " << timePercentage
                  << ", missing letters: " << missingLetters << ", letters%: " << lettersPercentage;

            p.points += 1000;
            p.points += timePercentage * 2000;
            p.points += missingLetters * 30;        // 20 pts when guessing each letter separately
            p.points += lettersPercentage * 100;
            p.points += ((double)p.health / (double)maxHealth) * 1000.0;    // Percent of remaining health * 1000
            PLOGD << "Player #" << player->id << ": points: " << p.points;
        }

        // Player made a wrong guess
        else {
            p.health -= 1;
            PLOGI << "Player #" << player->id << " failed to guessed the word '" << utf8word << "'";
        }
    }


    // True when all the players guessed or run out of health
    bool finished = std::find_if(players.begin(),
                                 players.end(),
                                 [] (const auto& p) {
                                     return !p.second.guessed && p.second.health > 0;
                                 }) == players.end();

    // Send a message to all the players
    if(finished) {
        PLOGD << "All players finished, finishing the game";
        sendScoreboard();
        teardownPlayers();
    } else {
        sendGameStatus();
    }

    return finished;
}

bool Game::guessLetter(Player *player, char32_t &letter) {
    PlayerInGame& p = players[player];

    PLOGI << "Player #" << player->id << " is trying to guess the letter '" << letter << "'";

    // If player hasn't guessed yet and has health left
    if(!p.guessed && p.health > 0) {

        // If player hasn't guessed this letter yet
        if(p.word.find(letter) == std::u32string::npos) {

            // Player correctly guessed the letter
            if(word.find(letter) != std::u32string::npos) {
                PLOGI << "Player #" << player->id << " correctly guessed the letter '" << letter << "'";

                uint8_t count = 0;
                for(unsigned int i = 0; i < word.size(); i++) {
                    if(word[i] == letter) {
                        p.word[i] = letter;
                        p.points += 20;         // 20 pts for each letter occurence
                        count += 1;
                    }
                }

                // Player guessed all the letters
                if(p.word.find('\0') == p.word.npos) {
                    p.guessed = true;

                    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
                    auto remaining = std::chrono::duration_cast<std::chrono::seconds>(endTime - std::chrono::system_clock::now()).count();

                    // 1.0 - player guessed the word at the beginning of the game
                    // 0.0 - player guessed the word at the end of the game
                    double timePercentage = (double)remaining / (double)duration;

                    // Letters that were missing (not guessed)
                    // 1.0 - all letters were missing
                    // 0.0 - none letters were missing
                    double lettersPercentage = (double)count / (double)p.word.size();

                    p.points += 1000;
                    p.points += timePercentage * 2000;
                    p.points += count * 10;                 // 20 pts when guessing each letter separately
                    p.points += lettersPercentage * 100;
                    p.points += ((double)p.health / (double)maxHealth) * 1000.0;    // Percent of remaining health * 1000

                }
            }

            // Player made a wrong guess
            else {
                // TODO: Log
                p.health -= 1;
            }

        }

        // Player already has this letter
        else {
            p.health -= 1;
            PLOGI << "Player #" << player->id << " failed to guessed the letter '" << letter << "'";
        }
    }


    // True when all the players guessed or run out of health
    bool finished = std::find_if(players.begin(),
                                 players.end(),
                                 [] (const auto& p) {
                                     return !p.second.guessed && p.second.health > 0;
                                 }) == players.end();

    // Send a message to all the players
    if(finished) {
        PLOGD << "All players finished, finishing the game";
        sendScoreboard();
        teardownPlayers();
    } else {
        sendGameStatus();
    }

    return finished;
}
