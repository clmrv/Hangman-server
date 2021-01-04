//
//  OutMessage.h
//  Hangman-server
//
//  Created by Przemek Ambroży on 28/12/2020.
//

#ifndef OutMessage_h
#define OutMessage_h

#include "Message.hpp"
#include "../Room.hpp"
#include "../Game.hpp"

#include <map>
#include <vector>
#include <set>
#include <string>

struct PossibleRoomSettings;
struct PlayerInGame;
struct RoomSettings;

namespace Message {

/// Create 'loggedIn' message
Out loggedIn(uint16_t id);

/// Create 'error' message
Out error(MessageError error);

/// Create 'roomSettings' message
Out roomSettings(PossibleRoomSettings& settings);

/// Create 'roomStatus' message
Out roomStatus(RoomSettings& settings, std::string& id, std::set<Player*>& players, Player* host);

/// Create 'kicked' message
Out kicked();

/// Create 'gameStatus' message
struct gameStatusBuilder {
private:
    std::vector<uint8_t> data;
    std::vector<uint8_t>::iterator wordPosition;
public:
    gameStatusBuilder(uint16_t remainingTime, std::map<Player*, PlayerInGame>& players, uint8_t wordLength);

    gameStatusBuilder& setWord(const std::u32string& word);

    Out build();
};


}




#endif /* OutMessage_h */
