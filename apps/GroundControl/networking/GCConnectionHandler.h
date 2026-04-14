// this it the .h file to implement a class to handle all the satellites that ground control is connected to

#ifndef GC_CONNECTION_HANDLER_C
#define GC_CONNECTION_HANDLER_C

#include <unordered_map>
#include "Connection.h"

class GCConnectionHandler {
    private:
        std::unordered_map<int, Connection> satellites; // where to store all peer connections

    public:
        void addIncomingConnection(int port, const std::string& ip, int satId); // a satellite trying to connect
        void addOutgoingConnection(int port, const std::string& ip, int satId); // connecting to a satellite
        void update();  // drives reconnection logic
        void removeConnection(int satId);
        Connection* getConnection(int satId); // non-owning, may return nullptr
        void sendMessageToSat(int satId, const Message& message);
        void broadcastMessage(const Message& message); // send a message to all peers
        void printAllSats(); // prints all the satellites connected to this satellite
};


#endif