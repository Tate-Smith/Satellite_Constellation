// the .h file for the ConnectionHandler class, which owns and manages all the peer connections in the network

#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include "PeerConnection.h"
#include "../logging/Logger.h"
#include "../concurrency/MessageQueue.h"
#include <unordered_map>

class ConnectionHandler {
    private:
        std::unordered_map<int, PeerConnection> connections; // where to store all peer connections
        MessageQueue *queue;

    public:
        ConnectionHandler(MessageQueue *queue); // constructor to instialize the queue
        void addIncomingConnection(int port, const std::string& ip, int peerId, int satId); // another satellite trying to connect
        void addOutgoingConnection(int port, const std::string& ip, int peerId, int satId); // connecting to another satellite
        void update();  // drives reconnection logic
        void removeConnection(int peerId);
        PeerConnection* getConnection(int peerId); // non-owning, may return nullptr
        void sendMessageToPeer(int peerId, const Message& message);
        void broadcastMessage(const Message& message); // send a message to all peers
        void printAllPeers(); // prints all the satellites connected to this satellite
};

#endif