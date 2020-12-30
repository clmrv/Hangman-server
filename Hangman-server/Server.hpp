//
//  Server.hpp
//  Hangman-server
//
//  Created by celmer on 22/12/2020.
//

#ifndef Server_hpp
#define Server_hpp

#include "Room.hpp"
#include "Player.hpp"
#include "Connection.hpp"
#include "OutMessage.hpp"
#include "InMessage.hpp"

#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <optional>
#include <algorithm>
#include <random>

struct AvailableRoomSettings {

};

struct RoomSettings {

};

class Server {
public:
    
    Server(int port);

    /// Creates a room
    /// @param host Player that will be the host
    /// @return Index of the room
    int createRoom(Player* host);
    
    ~Server(); 
    
    
private:
    std::map<int, Player> players;
    std::map<std::string, Room> rooms;

    std::map<int, Connection> connections;
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
    void joinRoom(Player& player, std::string id);

    /// Create (and join) a room
    /// @param player Player who wants to create a room
    /// @param settings Room settings
    void createRoom(Player& player, RoomSettings& settings);
};

#endif /* Server_hpp */
