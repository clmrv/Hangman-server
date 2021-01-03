//
//  OutMessage.cpp
//  Hangman-server
//
//  Created by Przemek Ambroży on 28/12/2020.
//

#include "OutMessage.hpp"

using namespace Message;

void push_back_uint16(std::vector<uint8_t>& vec, uint16_t value) {
    vec.push_back(uint8_t(value >> 8));
    vec.push_back(uint8_t(value & 0x00ff));
}

//
// DEBUG
//
//void printData(std::vector<uint8_t>& data) {
//    for(const auto& d : data) {
//        printf("%x ", d);
//    }
//    printf("\n");
//}


Out Message::loggedIn(uint16_t id) {

    uint16_t bigEndian = htons(id);

    return Out(MessageType::loggedIn,
               (const uint8_t*) &bigEndian,
               (size_t) sizeof(bigEndian));
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
    printf("PLAYER COUNT: %d-%d\n", settings.playerCount[0], data.back());

    return Out(MessageType::roomSettings, data.data(), data.size());

}


Out Message::roomStatus(RoomSettings &settings, std::string& id, std::set<Player*>& players, Player* host) {
    std::vector<uint8_t> data;

    data.insert(data.end(), settings.language.begin(), settings.language.end());
    data.push_back(settings.wordLength);
    push_back_uint16(data, settings.gameTime);
    data.push_back(settings.healthPoints);
    data.insert(data.end(), id.begin(), id.end());
    data.push_back(players.size());

    for(const auto& player : players) {
        push_back_uint16(data, player->id);
        std::string name = player->getName();
        data.push_back(name.size());
        data.insert(data.end(), name.begin(), name.end());
    }

    push_back_uint16(data, host->id);

    return Out(MessageType::roomStatus, data.data(), data.size());
}

gameStatusBuilder::gameStatusBuilder(uint16_t remainingTime,
                                     std::map<Player*, PlayerInGame>& players,
                                     uint8_t wordLength) {

    push_back_uint16(data, remainingTime);

    data.push_back(players.size());
    for(const auto& [player, inGame] : players) {
        push_back_uint16(data, player->id);
        data.push_back(player->getName().size());
        push_back_uint16(data, inGame.points);
        data.push_back(inGame.health);
        data.push_back(inGame.guessed ? 1 : 0);
    }

    data.push_back(wordLength);

    // Fill last (4 * wordLength) with zeros
    wordPosition = data.insert(data.end(), wordLength * 4, 0);
}

gameStatusBuilder& gameStatusBuilder::setWord(const std::u32string& word) {

    // Insert word instead of the empty one
    wordPosition = data.insert(wordPosition, word.begin(), word.end());
    return *this;
}

Out gameStatusBuilder::build() {
    return Out(MessageType::gameStatus, data.data(), data.size());
}

