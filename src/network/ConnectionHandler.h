// the .h file for the ConnectionHandler class, which owns and manages all the peer connections in the network

#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include "PeerConnection.h"
#include <unordered_map>
#include <iostream>
#include "../concurrency/MessageQueue.h"

class ConnectionHandler {
    private:
        std::unordered_map<int, PeerConnection> connections; // where to store all peer connections
        MessageQueue *queue;
        mutable std::mutex mtx;
        int listeningPort; // the port that this satellite is listening on

    public:
        ConnectionHandler(MessageQueue *queue, int listeningPort); // constructor to instialize the queue
        void addConnection(int port, const std::string& ip, int peerId, int satId); // a satellite or Ground Control trying to connect
        void update();  // drives reconnection logic
        void removeConnection(int peerId);
        bool hasConnection(int satId);
        void heartbeatSat(int satId); // sends a heartbeat and sets connection to connected
        void sendMessageToPeer(int peerId, const Message& message);
        void broadcastMessage(const Message& message); // send a message to all peers
        void printAllPeers(); // prints all the satellites connected to this satellite
};

#endif