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
        printf("Got message: Type: %d, Length: %d Data: %s\n", nextIn.type, nextIn.length, nextIn.data);
        incoming.push_back(std::move(nextIn));
    }

}

void Connection::write() {
    if(!outgoing.empty()) {
        if(outgoing.front().write(fd)) {
            outgoing.pop_front();
        }
    }
}
