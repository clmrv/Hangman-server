//
//  MessageType.h
//  Hangman-server
//
//  Created by Przemek Ambroży on 27/12/2020.
//

#ifndef MessageType_h
#define MessageType_h

enum class MessageType: uint8_t {
    unknown,
    login = 0x10,
    loggedIn = 0x11,
};


#endif /* MessageType_h */
