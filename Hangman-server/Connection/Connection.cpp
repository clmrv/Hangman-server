//
//  Connection.cpp
//  Hangman-server
//
//  Created by Przemek Ambroży on 27/12/2020.
//

#include "Connection.hpp"

Connection::Connection() {
    fd = 0;
    player = nullptr;
}

Connection::Connection(int fd) {
    this->fd = fd;
    player = nullptr;
}

void Connection::read() {

    if(nextIn.read(fd)) {
        printf("Got message: Type: 0x%x, Length: %d Data: %s\n", static_cast<uint8_t>(nextIn.type), nextIn.length, nextIn.data.data());
        incoming.push_back(std::move(nextIn));
        nextIn = Message::In();
    }

}

void Connection::write() {
    if(!outgoing.empty()) {
        if(outgoing.front().write(fd)) {
            outgoing.pop_front();
        }
    }
}
