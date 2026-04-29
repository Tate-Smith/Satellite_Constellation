// this it the .h file to implement a class to handle all the satellites that ground control is connected to

#ifndef GC_CONNECTION_HANDLER_H
#define GC_CONNECTION_HANDLER_H

#include <unordered_map>
#include <iostream>
#include <mutex>
#include "Connection.h"
#include "../output/Terminal.h"
#include "../../../src/concurrency/MessageQueue.h"

class GCConnectionHandler {
    private:
        std::unordered_map<int, Connection> satellites; // where to store all peer connections
        mutable std::mutex mtx;
        int gcPort; // the ground controls listing port
        Terminal &terminal;
        MessageQueue *queue;

    public:
        GCConnectionHandler(Terminal &terminal, MessageQueue *queue);
        void addConnection(int port, const std::string& ip, int satId, int gcPort); // connecting to a satellite
        void update();  // drives reconnection logic
        void removeConnection(int satId);
        bool hasConnection(int satId);
        void heartbeatSat(int satId); // sends a heartbeat and sets connection to connected
        void sendMessageToSat(int satId, const Message& message) const;
};


#endif