//
//  Config.cpp
//  Hangman-server
//
//  Created by Przemek Ambroży on 06/01/2021.
//

#include "Config.hpp"

Config::Config(const char* filename) {

    std::ifstream file(filename);

    if(!file.good()) {
        std::cout << "Error opening config file '" << filename << "'\n";
        return;
    }

    std::string buf;
    while(getline(file, buf)) {
        std::string_view line = buf;
        auto delimeter = line.find_first_of('=');
        auto key = line.substr(0, delimeter);
        auto value = std::string(line.substr(delimeter + 1));

        if(key == "port") {
            port = std::stoi(value);
        }
        else if(key == "langs") {

            roomSettings.languages.clear();

            std::istringstream iss(value);
            std::string lang;
            while(getline(iss, lang, ',')) {
                if(lang.size() == 2)
                    roomSettings.languages.push_back(lang);
            }

        }
        else if(key == "wordLength") {
            auto bar = value.find_first_of('-');
            roomSettings.wordLength[0] = std::stoi(value.substr(0, bar));
            roomSettings.wordLength[1] = std::stoi(value.substr(bar + 1));
        }
        else if(key == "gameTime") {
            auto bar = value.find_first_of('-');
            roomSettings.gameTime[0] = std::stoi(value.substr(0, bar));
            roomSettings.gameTime[1] = std::stoi(value.substr(bar + 1));
        }
        else if(key == "healthPoints") {
            auto bar = value.find_first_of('-');
            roomSettings.healthPoints[0] = std::stoi(value.substr(0, bar));
            roomSettings.healthPoints[1] = std::stoi(value.substr(bar + 1));
        }
        else if(key == "players") {
            auto bar = value.find_first_of('-');
            roomSettings.playerCount[0] = std::stoi(value.substr(0, bar));
            roomSettings.playerCount[1] = std::stoi(value.substr(bar + 1));
        }
        else {
            std::cout << "Undefined key during loading config: '" << key << "'\n";
        }

    }
    file.close();
}
