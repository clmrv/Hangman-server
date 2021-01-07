//
//  InMessage.hpp
//  Hangman-server
//
//  Created by Przemek Ambroży on 27/12/2020.
//

#ifndef Message_h
#define Message_h

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <array>
#include <vector>

#include "MessageType.h"
#include "../plog/Log.h"

namespace Message {


// MARK: - Incoming Message


class In {
private:
    std::array<uint8_t, 3> buf;
    uint32_t bufRead = 0;

public:

    MessageType type = MessageType::unknown;
    uint16_t length = 0;
    std::vector<uint8_t> data;

    bool error = false;

    /// Read message from a file descriptor
    /// @param fd File descriptor to read data from
    /// @return Whether the whole message was read
    bool read(int fd);

    /// Indicate whether the whole message was read
    bool completed();
};


// MARK: - Outgoing Message

class Out {
private:
    std::vector<uint8_t> bytes;
    size_t written = 0;

public:

    bool error = false;

    /// Create a message from bytes
    Out(MessageType type, const uint8_t* bytes, size_t count);

    /// Create a message from vector reference
    Out(MessageType type, const std::vector<uint8_t>& bytes);

    /// Write data to a descriptor
    /// @return Whether writing has been completed
    bool write(int fd);

    /// Check whether writing has been completed
    bool completed();

    /// Reset writing progress
    void reset();

};

}

#endif /* Message_h */
