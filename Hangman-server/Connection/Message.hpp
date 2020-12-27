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


// MARK: - InMessage


class InMessage {
private:
    uint8_t *buf;
    uint32_t bufRead = 0;

public:

    uint8_t type = 0;
    uint32_t length = 0;
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


#endif /* Message_h */
