//
//  InMessage.cpp
//  Hangman-server
//
//  Created by Przemek Ambroży on 28/12/2020.
//

#include "InMessage.hpp"

using namespace Message;

login::login(const In& message) {
    if(message.length == 2) {
        uint16_t id;
        memcpy(&id, message.data, sizeof(id));
        id = ntohs(id);
        this->id = id;
    } else {
        this->id = {};
    }
}
