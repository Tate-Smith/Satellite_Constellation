// this it the .h file to implement a class to handle all the satellites that ground control is connected to

#ifndef GC_CONNECTION_HANDLER_H
#define GC_CONNECTION_HANDLER_H

#include <unordered_map>
#include <iostream>
#include <mutex>
#include <cassert>
#include "Connection.h"
#include "../output/Terminal.h"
#include "../../concurrency/MessageQueue.h"
#include "../output/Parser.h"

class GCConnectionHandler {
    private:
        std::unordered_map<int, Connection> satellites; // where to store all peer connections
        mutable std::mutex mtx;
        int gcPort; // the ground controls listing port
        MessageQueue<std::string> *loggerQueue;
        MessageQueue<SatOutput> *outputQueue; // queue for the output values
        MessageQueue<CommandInput> *inputQueue; // queue for the commands from the terminal
        Parser parser; // parser object to deal with decoding the state for the terminal
        std::atomic<bool> *running;
        std::vector<char> emptyChunk; // for sending satellite status updates only

    public:
        GCConnectionHandler(MessageQueue<std::string> *loggerQueue, MessageQueue<SatOutput> *outputQueue, MessageQueue<CommandInput> *inputQueue, 
        std::atomic<bool> *running, int gcPort);
        void addConnection(int port, const std::string& ip, int satId); // connecting to a satellite
        void update();  // drives reconnection logic
        void removeConnection(int satId); // removes the connection from th emap
        bool hasConnection(int satId); // checks if the ground control is connected to that satellite
        void heartbeatSat(int satId); // sends a heartbeat and sets connection to connected
        void sendMessageToSat(int satId, const Message& message);
        void sendCommand(); // sends the command to a specific satellite
};


#endif