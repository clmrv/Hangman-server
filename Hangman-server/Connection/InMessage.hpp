//
//  InMessage.hpp
//  Hangman-server
//
//  Created by Przemek Ambroży on 28/12/2020.
//

#ifndef InMessage_hpp
#define InMessage_hpp

#include <optional>

#include "Message.hpp"

namespace Message {


struct login {
    std::optional<uint16_t> id;

    /// Create 'login' message
    login(const In& message);
};



}

#endif /* InMessage_hpp */
