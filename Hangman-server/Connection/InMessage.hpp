//
//  InMessage.hpp
//  Hangman-server
//
//  Created by Przemek Ambroży on 28/12/2020.
//

#ifndef InMessage_hpp
#define InMessage_hpp

#include <string>
#include <optional>

#include "Message.hpp"

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



}

#endif /* InMessage_hpp */
