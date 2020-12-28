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

class Server {
public:
    
    Server(int port);

    /// Creates a room
    /// @param host Player that will be the host
    /// @return Index of the room
    int createRoom(Player* host);
    
    ~Server(); 
    
    
private:
    std::vector<Player> players;
    std::vector<Room> rooms;

    std::map<int, Connection> connections;
    std::vector<pollfd> events;
    pollfd newEvent;
    
    int newConnectionSocket;
    
    void eventLoop();
    std::string generateRoomId();

    std::random_device rd;
    std::mt19937_64 generator { rd() };


    /// Create a new connection
    void connect();

    /// Connection ended
    /// @param fd Socket of the ended connection
    void disconnect(int fd);

    /// Login Player to the Connection
    /// @param conn Connection of the client that wants to log in
    /// @param restorationID Optional restoration ID of the player
    void loginPlayer(Connection& conn, std::optional<uint16_t> restorationID = {});
};

#endif /* Server_hpp */
