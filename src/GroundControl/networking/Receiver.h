// .h file to deal with recieving messages from satellites

#ifndef RECIEVER_H
#define RECIEVER_H

#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <unordered_map>
#include <vector>
#include <cassert>
#include "../../protocol/Message.h"
#include "../../protocol/Serializer.h"
#include "../../concurrency/MessageQueue.h"
#include "../datastorage/SatelliteData.h"
#include "GCConnectionHandler.h"
#include "../output/Terminal.h"
#include "../output/Parser.h"
#include "../../config/Config.h"

class Receiver {
    private:
        int serverSocket;
        sockaddr_in serverAddr;
        std::unordered_map<int, std::vector<char>> buffer; // this data structure is meant to accumulate the chunks of data from the 
        // satellite data downlink
        std::vector<SatelliteData> sats; // to hold all the data about the satellites
        MessageQueue<std::string> *loggerQueue; // logger queue
        MessageQueue<SatOutput> *outputQueue; // output queue
        Parser parser; // to parse file messages into structs for the terminal
        std::atomic<bool> *running;
        const int gcPort;
        std::vector<char> emptyChunk; // for when sending purely satellite staus updates (dead/alive) to the terminal

        void handleFileDump(const File_Msg& msg); // this private helper method handles the file dump coming from a satellite
        void addConnection(Message &message, GCConnectionHandler *handler, char ip[]); // adds a connection to handler if not already present
        void ack(Message &message, GCConnectionHandler *handler); // sends an ack to a satellite once its message is handled
        
    public:
        Receiver(const int gcPort, MessageQueue<std::string> *loggerQueue, MessageQueue<SatOutput> *outputQueue, std::atomic<bool> *running); // constructor
        void startServer(); // starts a server on port 8000
        void listen(GCConnectionHandler *handler); // listens for incoming messages
};

#endif