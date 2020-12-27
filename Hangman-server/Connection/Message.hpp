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

#include "MessageType.h"


// MARK: - InMessage


class InMessage {
private:
    uint8_t *buf;
    uint32_t bufRead = 0;

public:

    MessageType type = MessageType::unknown;
    uint16_t length = 0;
    uint8_t *data;

    bool error = false;

    InMessage();

    // Move constructor
    InMessage(InMessage&& other);

    ~InMessage();

    /// Read message from a file descriptor
    /// @param fd File descriptor to read data from
    /// @return Whether the whole message was read
    bool read(int fd);

    /// Indicate whether the whole message was read
    bool completed();
};


// MARK: - OutMessage

class OutMessage {
private:
    uint8_t* bytes;
    size_t count = 0;
    size_t written = 0;

public:

    bool error = false;

    /// Create a message from a text string
    OutMessage(MessageType type, const char* string);

    /// Create a message from bytes
    OutMessage(MessageType type, const uint8_t* bytes, size_t count);

    /// Move constructor
    OutMessage(OutMessage&& o);

    // Copy constructor
    OutMessage(const OutMessage& o);

    ~OutMessage();

    /// Write data to a descriptor
    /// @return Whether writing has been completed
    bool write(int fd);

    /// Check whether writing has been completed
    bool completed();

    /// Reset writing progress
    void reset();

};


#endif /* Message_h */
