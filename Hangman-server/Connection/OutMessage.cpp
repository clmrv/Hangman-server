//
//  OutMessage.cpp
//  Hangman-server
//
//  Created by Przemek Ambroży on 28/12/2020.
//

#include "OutMessage.hpp"

using namespace Message;

Out Message::loggedIn(uint16_t id) {

    uint16_t bigEndian = htons(id);

    return Out(MessageType::loggedIn,
               (const uint8_t*) &bigEndian,
               (size_t) sizeof(bigEndian));
}
