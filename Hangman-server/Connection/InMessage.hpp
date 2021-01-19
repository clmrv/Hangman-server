//
//  InMessage.hpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 28/12/2020.
//

#ifndef InMessage_hpp
#define InMessage_hpp

#include <string>
#include <optional>

#include "Message.hpp"
#include "../Room.hpp"

namespace Message {


struct login {
    std::optional<uint16_t> id;

    /// Create 'login' message
    login(const In& message);
};

struct setName {
    std::string name;

    /// Create 'setName' message
    setName(const In& message);
};

struct joinRoom {
    std::string id;

    /// Create 'joinRoom' message
    joinRoom(const In& message);
};

struct createRoom {
    RoomSettings settings;

    /// Create 'createRoom' message
    createRoom(const In& message);
};

struct setNewHost {
    uint16_t id;

    /// Create 'setNewHost' message
    setNewHost(const In& message);
};

struct kickPlayer {
    uint16_t id;

    /// Create 'kickPlayer' message
    kickPlayer(const In& message);
};

struct guessWord {
    std::u32string word;

    /// Create 'guessWord' message
    guessWord(const In& message);
};

struct guessLetter {
    char32_t letter;

    /// Create 'guessLetter' message
    guessLetter(const In& message);
};




}

#endif /* InMessage_hpp */
