// .h file to deal with recieving messages from satellites

#ifndef RECIEVER_H
#define RECIEVER_H

#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <unordered_map>
#include <vector>
#include "../../../src/protocol/Message.h"
#include "../../../src/protocol/Serializer.h"
#include "../../../src/concurrency/MessageQueue.h"
#include "../datastorage/SatelliteData.h"
#include "GCConnectionHandler.h"
#include "../output/Terminal.h"
#include "../output/Parser.h"

class Receiver {
    private:
        int serverSocket;
        sockaddr_in serverAddr;
        std::unordered_map<int, std::vector<char>> buffer; // this data structure is meant to accumulate the chunks of data from the 
        // satellite data downlink
        std::vector<SatelliteData> sats; // to hold all the data about the satellites
        MessageQueue<std::string> *logger_queue; // logger queue
        MessageQueue<SatOutput> *output_queue; // output queue
        Parser parser;

        void handleFileDump(const File_Msg& msg); // this private helper method handles the file dump coming from a satellite
        
    public:
        Receiver(MessageQueue<std::string> *logger_queue, MessageQueue<SatOutput> *output_queue); // constructor
        void startServer(); // starts a server on port 8000
        void listen(GCConnectionHandler *handler); // listens for incoming messages
};

#endif