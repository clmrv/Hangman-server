//
//  Config.hpp
//  Hangman-server
//
//  Created by Przemek Ambroży on 06/01/2021.
//

#ifndef Config_hpp
#define Config_hpp

#include <string>
#include <sstream>
#include <fstream>
#include "Room.hpp"


/// Server configuration
struct Config {
    /// Port number
    uint16_t port = 1234;

    /// Possible room settings
    PossibleRoomSettings roomSettings = {
        .languages = { "pl", "us" },
        .wordLength = { 3, 13 },
        .gameTime = { 30, 300 },
        .healthPoints = { 1, 8 },
        .playerCount = { 2, 5 }
    };


    /// Read the configuration from a file
    /// @param filename Name of the file containing the configuration
    /// @discussion Example file contents:
    /// @code
    ///     port=1234
    ///     langs=pl,en
    ///     wordLength=3-13
    ///     gameTime=30-300
    ///     healthPoints=1-10
    ///     players=2-5
    /// @endcode
    Config(const char* filename);
};

#endif /* Config_hpp */
