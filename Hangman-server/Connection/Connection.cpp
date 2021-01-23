//
//  Connection.cpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 27/12/2020.
//

#include "Connection.hpp"

Connection::Connection() {
    fd = 0;
    player = nullptr;
    PLOGD << "Creating empty connection";
}

Connection::Connection(int fd) {
    this->fd = fd;
    player = nullptr;
    PLOGD << "Creating connection with FD: " << fd;
}

void Connection::read() {
    try {
        if(nextIn.read(fd)) {
            PLOGD << "Connection FD: " << fd << " received complete message of type 0x"
                  << std::hex << (int)static_cast<uint8_t>(nextIn.type) << " and length " << std::dec << nextIn.length;
            incoming.push_back(std::move(nextIn));
            nextIn = Message::In();
        }
    } catch (const std::exception& e) {
        throw;
    }
}

void Connection::write() {
    if(!outgoing.empty()) {
        try {
            if(outgoing.front().write(fd)) {
                PLOGD << "Connection FD: " << fd << " completed sending a message";
                outgoing.pop_front();
            }
        } catch (const std::exception& e) {
            throw;
        }
    }
}
