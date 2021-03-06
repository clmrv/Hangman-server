//
//  Config.cpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 06/01/2021.
//

#include "Config.hpp"

Config::Config(const char* filename) {

    PLOGD << "Reading config from file '" << filename << "'";

    std::ifstream file;
    
    try {
        file.open(filename);
        if(!file.good()) {
            PLOGE << "Error opening file " << filename << ", using default settings";
            return;
        }
    } catch (const std::exception& e) { PLOGW << "Could not load config file"; }

    std::string buf;
    while(getline(file, buf)) {
        std::string_view line = buf;
        auto delimeter = line.find_first_of('=');
        auto key = line.substr(0, delimeter);
        auto value = std::string(line.substr(delimeter + 1));

        if(key == "port") {
            try {
                port = std::stoi(value);
                PLOGV << "\tport: " << port;
            } catch (const std::exception& e) { PLOGW << "Could not load port from config"; }
        }
        else if(key == "langs") {

            roomSettings.languages.clear();

            std::istringstream iss(value);
            std::string lang;
            while(getline(iss, lang, ',')) {
                if(lang.size() == 2) {
                    roomSettings.languages.push_back(lang);
                    PLOGV << "\tlang: " << lang;
                }
            }

        }
        else if(key == "wordLength") {
            try {
                auto bar = value.find_first_of('-');
                roomSettings.wordLength[0] = std::stoi(value.substr(0, bar));
                roomSettings.wordLength[1] = std::stoi(value.substr(bar + 1));
                PLOGV << "\tword length: " << (int)roomSettings.wordLength[0] << " - " << (int)roomSettings.wordLength[1];
            } catch (const std::exception& e) { PLOGW << "Could not load wordLength from config"; }
        }
        else if(key == "gameTime") {
            try {
                auto bar = value.find_first_of('-');
                roomSettings.gameTime[0] = std::stoi(value.substr(0, bar));
                roomSettings.gameTime[1] = std::stoi(value.substr(bar + 1));
                PLOGV << "\tgame time: " << (int)roomSettings.gameTime[0] << " - " << (int)roomSettings.gameTime[1] << " seconds";
            } catch (const std::exception& e) { PLOGW << "Could not load gameTime from config"; }
        }
        else if(key == "healthPoints") {
            try {
                auto bar = value.find_first_of('-');
                roomSettings.healthPoints[0] = std::stoi(value.substr(0, bar));
                roomSettings.healthPoints[1] = std::stoi(value.substr(bar + 1));
                PLOGV << "\thealth points: " << (int)roomSettings.healthPoints[0] << " - " << (int)roomSettings.healthPoints[1];
            } catch (const std::exception& e) { PLOGW << "Could not load healthPoints from config"; }
        }
        else if(key == "players") {
            try {
                auto bar = value.find_first_of('-');
                roomSettings.playerCount[0] = std::stoi(value.substr(0, bar));
                roomSettings.playerCount[1] = std::stoi(value.substr(bar + 1));
                PLOGV << "\tplayers: " << (int)roomSettings.playerCount[0] << " - " << (int)roomSettings.playerCount[1];
            } catch (const std::exception& e) { PLOGW << "Could not load players from config"; }
        }
        else {
            PLOGV << "Undefined key udring loading config: '" << key << "'";
        }

    }
    file.close();
}
