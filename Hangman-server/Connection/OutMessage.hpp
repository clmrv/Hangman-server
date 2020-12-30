//
//  OutMessage.h
//  Hangman-server
//
//  Created by Przemek Ambroży on 28/12/2020.
//

#ifndef OutMessage_h
#define OutMessage_h

#include "Message.hpp"
#include "Room.hpp"

class PossibleRoomSettings;

namespace Message {

/// Create 'loggedIn' message
Out loggedIn(uint16_t id);

/// Create 'error' message
Out error(MessageError error);

/// Create 'roomSettings' message
Out roomSettings(PossibleRoomSettings& settings);

}




#endif /* OutMessage_h */
