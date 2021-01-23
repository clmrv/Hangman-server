//
//  Message.cpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 27/12/2020.
//

#include "Message.hpp"

using namespace Message;


// MARK: - InMessage


bool In::read(int fd) {
    ssize_t n;

    // Read header parts
    if(bufRead < 3) {
        n = ::read(fd, &buf[bufRead], 3 - bufRead);

        // Did read something
        if(n >= 0) {
            bufRead += n;

            // Got complete header
            if(bufRead == 3) {

                // Type
                type = static_cast<MessageType>(buf[0]);

                // Length
                length = (uint16_t)buf[1] << 8 | (uint16_t)buf[2];

                // Data
                data.resize(length);
            }
        }

        // Error reading
        else {
            PLOGE << "Error reading message from FD: " << fd;
            throw new std::exception();
            return true;
        }
    }

    // Read data parts
    else if(bufRead < length + 3u) {
        n = ::read(fd, &data[bufRead - 3], length + 3 - bufRead);

        // Did read something
        if(n >= 0) {
            bufRead += n;
        }

        // Error reading
        else {
            PLOGE << "Error reading message from FD: " << fd;
            throw new std::exception();
            return true;
        }
    }

    return (bufRead >= 3 && bufRead == length + 3u );
}



// MARK: - OutMessage


/// Create a message from bytes
Out::Out(MessageType type, const uint8_t* bytes, size_t count) {
    this->bytes.reserve(count + 3);
    this->bytes.push_back(static_cast<uint8_t>(type));
    this->bytes.push_back(uint8_t(count >> 8));
    this->bytes.push_back(uint8_t(count & 0x00ff));
    this->bytes.insert(this->bytes.end(), bytes, bytes + count);
}

/// Create a message from vector reference
Out::Out(MessageType type, const std::vector<uint8_t>& bytes) {
    this->bytes.reserve(bytes.size() + 3);
    this->bytes.push_back(static_cast<uint8_t>(type));
    this->bytes.push_back(uint8_t(bytes.size() >> 8));
    this->bytes.push_back(uint8_t(bytes.size() & 0x00ff));
    this->bytes.insert(this->bytes.end(), bytes.begin(), bytes.end());
}

/// Write data to a descriptor
/// @return Whether writing has been completed
bool Out::write(int fd) {
    ssize_t n = ::write(fd, &bytes[written], bytes.size() - written);
    if(n >= 0) {
        written += static_cast<size_t>(n);
    } else {
        PLOGE << "Error writing message to FD: " << fd;
        error = true;
        return true;
    }

    return written == bytes.size();
}

/// Check whether writing has been completed
bool Out::completed() {
    return error || bytes.size() == written;
}

/// Reset writing progress and error
void Out::reset() {
    written = 0;
    error = false;
}
