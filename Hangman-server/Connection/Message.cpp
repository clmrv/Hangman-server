//
//  Message.cpp
//  Hangman-server
//
//  Created by Przemek Ambroży on 27/12/2020.
//

#include "Message.hpp"


// MARK: - InMessage


InMessage::InMessage() {
    buf = new uint8_t[5];
}

InMessage::InMessage(InMessage&& other) {
    type = other.type;
    length = other.length;
    data = other.data;
    error = other.error;
    buf = other.buf;
    bufRead = other.bufRead;

    other.type = 0;
    other.length = 0;
    other.data = nullptr;
    other.error = false;
    other.buf = new uint8_t;
    other.bufRead = 0;
}

InMessage::~InMessage() {
    delete [] buf;
    delete [] data;
}

bool InMessage::read(int fd) {
    ssize_t n;

    // Read header parts
    if(bufRead < 5) {
        n = ::read(fd, buf + bufRead, 5 - bufRead);

        // Did read something
        if(n >= 0) {
            bufRead += n;

            // Got complete header
            if(bufRead == 5) {

                // Type
                type = buf[0];

                // Length
                memcpy(&length, &buf[1], sizeof(length));
                length = ntohs(length);
                // TODO: Delete this line \/
                length = 5;

                // Data
                if(length > 0) {
                    data = new uint8_t[length];
                } else {
                    data = nullptr;
                }
            }
        }

        // Error reading
        else {
            error = true;
            return true;
        }
    }

    // Read data parts
    else if(bufRead < length + 5) {
        n = ::read(fd, data + bufRead - 5, length + 5 - bufRead);

        // Did read something
        if(n >= 0) {
            bufRead += n;
        }

        // Error reading
        else {
            error = true;
            return true;
        }
    }

    return error || (bufRead >= 5 && bufRead == length + 5 );
}

bool InMessage::completed() {
    return error || (bufRead >= 5 && bufRead == length + 5 );
}


// MARK: - OutMessage

