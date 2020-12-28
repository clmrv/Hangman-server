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
        uint16_t restorationID;
        memcpy(&restorationID, message.data, sizeof(restorationID));
        restorationID = ntohs(restorationID);
        this->restorationID = restorationID;
    } else {
        this->restorationID = {};
    }
}
