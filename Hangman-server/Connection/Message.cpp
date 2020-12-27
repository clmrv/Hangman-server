//
//  Message.cpp
//  Hangman-server
//
//  Created by Przemek Ambroży on 27/12/2020.
//

#include "Message.hpp"


// MARK: - InMessage


InMessage::InMessage() {
    buf = new uint8_t[3];
}

InMessage::InMessage(InMessage&& other) {
    type = other.type;
    length = other.length;
    data = other.data;
    error = other.error;
    buf = other.buf;
    bufRead = other.bufRead;

    other.type = MessageType::unknown;
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
    if(bufRead < 3) {
        n = ::read(fd, buf + bufRead, 3 - bufRead);

        // Did read something
        if(n >= 0) {
            bufRead += n;

            // Got complete header
            if(bufRead == 3) {

                // Type
                type = static_cast<MessageType>(buf[0]);

                // Length
                memcpy(&length, &buf[1], sizeof(length));
                length = ntohs(length);

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
    else if(bufRead < length + 3) {
        n = ::read(fd, data + bufRead - 3, length + 3 - bufRead);

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

    return error || (bufRead >= 3 && bufRead == length + 3 );
}

bool InMessage::completed() {
    return error || (bufRead >= 3 && bufRead == length + 3 );
}


// MARK: - OutMessage

/// Create a message from a text string
OutMessage::OutMessage(MessageType type, const char* string) {
    count = strlen(string) + 1 + 3;
    bytes = new uint8_t[count];
    bytes[0] = static_cast<uint8_t>(type);
    uint16_t len = htons((uint16_t)count - 3);
    memcpy(this->bytes + 1, &len, sizeof(len));
    memcpy(bytes + 3, string, count);
}

/// Create a message from bytes
OutMessage::OutMessage(MessageType type, const uint8_t* bytes, size_t count) {
    this->count = count + 3;
    this->bytes = new uint8_t[count];
    this->bytes[0] = static_cast<uint8_t>(type);
    uint16_t len = htons((uint16_t)count);
    memcpy(this->bytes + 1, &len, sizeof(len));
    memcpy(this->bytes + 3, bytes, count);
}

/// Move constructor
OutMessage::OutMessage(OutMessage&& o) {
    bytes = o.bytes;
    count = o.count;
    written = o.written;
    error = o.error;
    o.bytes = nullptr;
    o.count = 0;
    o.written = 0;
    o.error = false;
}

// Copy constructor
OutMessage::OutMessage(const OutMessage& o) {
    count = o.count;
    written = o.written;
    error = o.error;
    bytes = new uint8_t[count];
    memcpy(bytes, o.bytes, count);
}

OutMessage::~OutMessage() {
    delete [] bytes;
}

/// Write data to a descriptor
/// @return Whether writing has been completed
bool OutMessage::write(int fd) {
    ssize_t n = ::write(fd, bytes + written, count - written);
    if(n >= 0) {
        written += static_cast<size_t>(n);
    } else {
        error = true;
        return true;
    }

    return written == count;
}

/// Check whether writing has been completed
bool OutMessage::completed() {
    return error || count == written;
}

/// Reset writing progress and error
void OutMessage::reset() {
    written = 0;
    error = false;
}
