//
//  MessageType.h
//  Hangman-server
//
//  Created by Przemek Ambroży on 27/12/2020.
//

#ifndef MessageType_h
#define MessageType_h

enum class MessageType: uint8_t {
    unknown = 0x00,

    // K > S
    login = 0x11,
    setName = 0x12,
    joinRoom = 0x21,
    createRoom = 0x22,
    setNewHost = 0x23,
    kickPlayer = 0x24,
    leaveRoom = 0x25,
    startGame = 0x26,
    guessWord = 0x31,
    guessLetter = 0x32,

    // S > K
    loggedIn = 0x91,
    error = 0x81,
    roomSettings = 0xa1,
    roomStatus = 0xa2,
    gameStatus = 0xb1,
    scoreBoard = 0xb2,
};


#endif /* MessageType_h */
