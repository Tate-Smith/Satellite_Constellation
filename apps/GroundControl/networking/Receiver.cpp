/*
File: Reciever
Date Created: April 9th, 2026
Last Updated: May 2nd, 2026
Author: Tate Smith
Purpose: This file is the listener for the ground control station, it starts a server to listen from,
then will listen for incoming messages and handles them
*/

#include "Receiver.h"

static const int PORT = 8000; // always listening on port 8000
static const int BUFFER = 2048;

Receiver::Receiver(MessageQueue<std::string> *logger_queue, MessageQueue<SatOutput> *output_queue) : serverSocket(-1), logger_queue(logger_queue), output_queue(output_queue) {}

void Receiver::handleFileDump(const File_Msg& msg) {
    // This methods job is to handle the file dump incoming from a satellite
    auto& chunk = this->buffer[msg.senderId];
    chunk.insert(chunk.end(), msg.data, msg.data + msg.len);

    // if it is the last chunk of data
    if (msg.last) {
        // parse this chunk and then add it to the queue
        output_queue->pushBack(parser.dataDecoder(msg.senderId, chunk, true));
        chunk.clear();
    }
}

void Receiver::startServer() {
    // function to start a server on the specified port
    // creates an IPv4 UDP socket and returns a file descriptor for the socket
    this->serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    // if the file descriptor is negative then there was an error creating the socket
    if (serverSocket < 0) {
        logger_queue->pushBack("Error creating socket");
        return;
    }
    // zeroes out the serverAddr struct
    memset(&serverAddr, 0, sizeof(serverAddr));

    // configures the port with network byte order, and accepts connections from any interface
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    // binds the socket to a certain port to listen for messages, if it is negative then there was an error
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        logger_queue->pushBack("Error binding socket");
        return;
    }
    // set a timeout to prevent blocking
    struct timeval timeout;
    timeout.tv_sec = 0; 
    timeout.tv_usec = 100000; // 100ms
    setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    logger_queue->pushBack("Server started on port: " + std::to_string(PORT));
}   

void Receiver::listen(GCConnectionHandler *handler) {
    while (true) {
        // function to connect to another peer
        // buffer for the message
        char buffer [BUFFER];
        sockaddr_in senderAddr;
        socklen_t addrlen = sizeof(senderAddr);
        // zeroes out the senderAddr struct
        memset(&senderAddr, 0, addrlen);

        // recieve bytes
        int bytesReceived = recvfrom(this->serverSocket, buffer, sizeof(buffer), 0, (sockaddr*)&senderAddr, &addrlen);
        if (bytesReceived < 0) continue;

        // get the sender ip, and create or find a connection
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &senderAddr.sin_addr, ip, sizeof(ip));

        // convert the bytes to a message
        std::unique_ptr<Message> msg;
        try {
            msg = decode(reinterpret_cast<uint8_t*>(buffer), bytesReceived);
        }
        catch (...) {
            // skip the udp packet if it isnt decoded properly
            continue;
        }

        Message& message = *msg;

       // add if not already known
        if (!handler->hasConnection(message.senderId)) handler->addConnection(message.senderPort, ip, message.senderId, PORT);
        else {
            // if known update connected status and heartbeat
            handler->heartbeatSat(message.senderId);
            std::vector<char> empty;
            output_queue->pushBack(parser.dataDecoder(message.senderId, empty, true));
        }

        logger_queue->pushBack("Message received from Satellite Id: " + std::to_string(message.senderId));

        // keep track of whether a message was properly handled or not
        bool handled = false;
        // decide what to do with the message
        if (message.header.type == MessageType::FILE_MSG) {
            // if it is a file dump
            handleFileDump(static_cast<const File_Msg&>(*msg));
            handled = true;
        }
        else if (message.header.type == MessageType::HEARTBEAT) {
            // if it is a heartbeat
            handled = true;
        }

        // send an ack message to the given satellite let it know the message was recieved if the message was properly handled
        if (handled) {
            Ack m;
            m.header.size = sizeof(m);
            m.header.type = MessageType::ACK;
            m.senderId = 0;
            m.senderPort = PORT;
            m.received = true;
            handler->sendMessageToSat(message.senderId, m);
        }
    }
}