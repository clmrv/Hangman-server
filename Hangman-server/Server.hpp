//
//  Server.hpp
//  Hangman-server
//
//  Created by Przemysław Ambroży and Błażej Celmer on 22/12/2020.
//

#ifndef Server_hpp
#define Server_hpp

#include "Config.hpp"
#include "Room.hpp"
#include "Player.hpp"
#include "Connection/Connection.hpp"
#include "Connection/OutMessage.hpp"
#include "Connection/InMessage.hpp"
#include "plog/Log.h"

#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unordered_map>
#include <optional>
#include <algorithm>
#include <random>
#include <chrono>

class Server {
public:
    
    Server();

    /// Start the server
    void start();

    /// Stop the server
    void stop();
    
    ~Server();
    
private:
    std::unordered_map<uint16_t, Player> players;
    std::unordered_map<std::string, Room> rooms;
    std::unordered_map<std::string, Game> games;

    Config config;
    std::unordered_map<int, Connection> connections;
    std::vector<pollfd> events;
    pollfd newEvent;
    
    int newConnectionSocket;
    
    void eventLoop();
    std::string generateRoomId();

    std::random_device rd;
    std::mt19937_64 generator { rd() };

    // Messages

    /// Handle messages from every connection
    void handleMessages();

    /// Handle every game
    void handleGames();

    /// Create a new connection
    void connect();

    /// Close a connection
    /// @param fd Socket of the closed connection
    void disconnect(int fd);

    /// Login Player to the Connection
    /// @param conn Connection of the client that wants to log in
    /// @param existingID Optional player ID
    void login(Connection& conn, std::optional<uint16_t> existingID = {});

    /// Join a room
    /// @param player Player which wants to join
    /// @param id 6-digit code of a room
    void joinRoom(Player* player, std::string id);

    /// Create (and join) a room
    /// @param player Player who wants to create a room
    /// @param settings Room settings
    void createRoom(Player* player, RoomSettings& settings);

    /// Start the game
    /// @param room Room for the game
    void startGame(Room& room);

    /// Calculate time when next game finished (or -1 when there are no games)
    int nextGameLoopTime();
};

#endif /* Server_hpp */
