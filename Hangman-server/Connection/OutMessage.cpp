//
//  OutMessage.cpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 28/12/2020.
//

#include "OutMessage.hpp"

using namespace Message;

void push_back_uint64(std::vector<uint8_t>& vec, uint64_t value) {
    vec.push_back(uint8_t((value >> 56) & 0xFFull));
    vec.push_back(uint8_t((value >> 48) & 0xFFull));
    vec.push_back(uint8_t((value >> 40) & 0xFFull));
    vec.push_back(uint8_t((value >> 32) & 0xFFull));
    vec.push_back(uint8_t((value >> 24) & 0xFFull));
    vec.push_back(uint8_t((value >> 16) & 0xFFull));
    vec.push_back(uint8_t((value >> 8) & 0xFFull));
    vec.push_back(uint8_t(value & 0xFFull));
}

void push_back_uint32(std::vector<uint8_t>& vec, uint32_t value) {
    vec.push_back(uint8_t((value >> 24) & 0xFFul));
    vec.push_back(uint8_t((value >> 16) & 0xFFul));
    vec.push_back(uint8_t((value >> 8) & 0xFFul));
    vec.push_back(uint8_t(value & 0xFFul));
}

void push_back_uint16(std::vector<uint8_t>& vec, uint16_t value) {
    vec.push_back(uint8_t(value >> 8));
    vec.push_back(uint8_t(value & 0x00ff));
}


Out Message::loggedIn(uint16_t id) {

    std::vector<uint8_t> bigEndian { (uint8_t)(id >> 8), (uint8_t)(id & 0x00ff) };
    
    return Out(MessageType::loggedIn, bigEndian);
}

Out Message::error(MessageError error) {
    
    uint8_t errorCode = static_cast<uint8_t>(error);

    return Out(MessageType::error, &errorCode, 1);
}

Out Message::roomSettings(PossibleRoomSettings& settings) {

    std::vector<uint8_t> data;

    data.push_back(settings.languages.size());
    for(const auto& lang : settings.languages) {
        //data.insert(data.end(), lang.c_str(), lang.c_str() + 2);
        data.insert(data.end(), lang.begin(), lang.end());
    }
    
    data.insert(data.end(), settings.wordLength.begin(), settings.wordLength.end());
    push_back_uint16(data, settings.gameTime[0]);
    push_back_uint16(data, settings.gameTime[1]);
    data.insert(data.end(), settings.healthPoints.begin(), settings.healthPoints.end());
    data.insert(data.end(), settings.playerCount.begin(), settings.playerCount.end());

    return Out(MessageType::roomSettings, data);
}


Out Message::roomStatus(RoomSettings &settings, std::string& id, std::set<Player*>& players, Player* host) {
    std::vector<uint8_t> data;

    data.insert(data.end(), settings.language.begin(), settings.language.end());
    data.push_back(settings.wordLength);
    push_back_uint16(data, settings.gameTime);
    data.push_back(settings.healthPoints);
    data.insert(data.end(), id.begin(), id.end());
    data.push_back(settings.maxPlayers);
    data.push_back(players.size());

    for(const auto& player : players) {
        push_back_uint16(data, player->id);
        std::string name = player->getName();
        data.push_back(name.size());
        data.insert(data.end(), name.begin(), name.end());
    }

    push_back_uint16(data, host->id);

    return Out(MessageType::roomStatus, data);
}

Out Message::kicked() {
    return Out(MessageType::kicked, std::vector<uint8_t>());
}

gameStatusBuilder::gameStatusBuilder(uint64_t endTime,
                                     std::map<Player*, PlayerInGame>& players,
                                     uint8_t wordLength) {

    push_back_uint64(data, endTime);

    data.push_back(players.size());
    for(const auto& [player, inGame] : players) {
        push_back_uint16(data, player->id);
        std::string name = player->getName();
        data.push_back(name.size());
        data.insert(data.end(), name.begin(), name.end());
        push_back_uint16(data, inGame.points);
        data.push_back(inGame.health);
        data.push_back(inGame.guessed ? 1 : 0);
    }

    data.push_back(wordLength);

    // Fill last (4 * wordLength) with zeros
    wordPosition = data.insert(data.end(), wordLength * 4, 0);
}

gameStatusBuilder& gameStatusBuilder::setWord(const std::u32string& word) {

    std::vector<uint8_t> raw;
    raw.reserve(word.size() * 4);
    for(const auto& letter : word) {
        push_back_uint32(raw, letter);
    }

    // Insert word instead of the empty one
    wordPosition = data.insert(wordPosition, raw.begin(), raw.end());
    return *this;
}

Out gameStatusBuilder::build() {
    return Out(MessageType::gameStatus, data);
}

Out Message::scoreboard(std::vector<PlayerInGame>& players, const std::u32string& word) {
    std::vector<uint8_t> data;

    data.push_back(players.size());

    for(const auto& player : players) {
        push_back_uint16(data, player.player->id);
        std::string name = player.player->getName();
        data.push_back(name.size());
        data.insert(data.end(), name.begin(), name.end());
        push_back_uint16(data, player.points);
        data.push_back(player.guessed ? 1 : 0);
    }

    data.push_back(word.size());

    std::vector<uint8_t> raw;
    raw.reserve(word.size() * 4);
    for(const auto& letter : word) {
        push_back_uint32(raw, letter);
    }
    data.insert(data.end(), raw.begin(), raw.end());

    return Out(MessageType::scoreBoard, data);
}
