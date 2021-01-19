//
//  InMessage.cpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 28/12/2020.
//

#include "InMessage.hpp"
#include <string>

using namespace Message;

uint16_t big_endian(const uint8_t& first, const uint8_t& second) {
    return (((uint16_t)first) << 8) | ((uint16_t)second);
}

uint32_t big_endian(const uint8_t& first, const uint8_t& second, const uint8_t& third, const uint8_t& fourth) {
    return (((uint32_t)first) << 14) | (((uint32_t)second) << 16) | (((uint32_t)third) << 8) | ((uint32_t)fourth);
}

login::login(const In& message) {
    if(message.length == 2) {
        this->id = big_endian(message.data[0], message.data[1]);
    } else {
        this->id = {};
    }
}

setName::setName(const In& message) {
    name = std::string(message.data.begin(), message.data.end());
}

joinRoom::joinRoom(const In& message) {
    id = std::string(message.data.begin(), message.data.end());
}

createRoom::createRoom(const In& message) {
    settings = RoomSettings {
        std::string(message.data.begin(), message.data.begin() + 2),    // Language
        message.data[2],                                // Word length
        big_endian(message.data[3], message.data[4]),   // Game time
        message.data[5],                                // Health points
        message.data[6]                                 // maxPlayers
    };
}

setNewHost::setNewHost(const In& message) {
    id = big_endian(message.data[0], message.data[1]);
}

kickPlayer::kickPlayer(const In& message) {
    id = big_endian(message.data[0], message.data[1]);
}

guessWord::guessWord(const In& message) {

    char32_t data[message.length / 4];
    for(int i = 0; i < message.length; i += 4) {
        data[i / 4] = big_endian(message.data[i],
                                 message.data[i+1],
                                 message.data[i+2],
                                 message.data[i+3]);
    }

    word = std::u32string(data, message.length / 4);
}

guessLetter::guessLetter(const In& message) {
    letter = big_endian(message.data[0],
                        message.data[1],
                        message.data[2],
                        message.data[3]);
}
