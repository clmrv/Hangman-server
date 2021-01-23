//
//  main.cpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 22/12/2020.
//

#include <iostream>
#include "plog/Log.h"
#include "plog/Initializers/RollingFileInitializer.h"
#include "plog/Appenders/RollingFileAppender.h"
#include "plog/Appenders/ConsoleAppender.h"
#include "Server.hpp"
#include "Connection/Connection.hpp"


int main(int argc, const char * argv[]) {

    // Initialize a logger
    static plog::RollingFileAppender<plog::TxtFormatterEmoji> fileAppender("data.log", 1000000, 1);
    static plog::ConsoleAppender<plog::TxtFormatterEmoji> consoleAppender;
    plog::init(plog::verbose, &fileAppender).addAppender(&consoleAppender);

    // Initialize a server
    Server server;
    server.start();

    return 0;
}
