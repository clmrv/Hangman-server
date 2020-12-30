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
        data.insert(data.end(), lang.c_str(), lang.c_str() + 2);
    }
    
    data.insert(data.end(), settings.wordLength, settings.wordLength + 2);
    push_back_uint16(data, settings.gameTime[0]);
    push_back_uint16(data, settings.gameTime[1]);
    data.insert(data.end(), settings.healthPoints, settings.healthPoints + 2);
    data.insert(data.end(), settings.playerCount, settings.playerCount + 2);


    return Out(MessageType::roomSettings, data.data(), data.size());

}
