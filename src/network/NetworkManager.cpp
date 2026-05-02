/*
File: NetworkManager
Date Created: March 28th, 2026
Last Updated: May 1st, 2026
Purpose: This file contains the implementation for the NetworkManager class, which is responsible for handling all network listening
It can start a server, and accept connections from other peers, and it uses the ConnectionHandler to manage the connections and messages
*/

#include "NetworkManager.h"

static const int BUFFER = 2048;

NetworkManager::NetworkManager(MessageQueue<std::string> *queue, Satellite *self, Logger *logger, int satId) : serverSocket(-1), satId(satId), queue(queue),
self(self), logger(logger) {}

void NetworkManager::startServer(int port) {
    // function to start a server on the specified port
    // creates an IPv4 UDP socket and returns a file descriptor for the socket
    this->serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    // if the file descriptor is negative then there was an error creating the socket
    if (serverSocket < 0) {
        queue->pushBack("[ERROR] Error creating socket");
        return;
    }
    // zeroes out the serverAddr struct
    memset(&serverAddr, 0, sizeof(serverAddr));

    // configures the port with network byte order, and accepts connections from any interface
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    // binds the socket to a certain port to listen for messages, if it is negative then there was an error
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        queue->pushBack("[ERROR] Error binding socket");
        return;
    }
    // set a timeout to prevent blocking
    struct timeval timeout;
    timeout.tv_sec = 0; 
    timeout.tv_usec = 100000; // 100ms
    setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    queue->pushBack("[NETWORK] Server started on port: " + std::to_string(port));
}   

void NetworkManager::acceptConnections(ConnectionHandler *handler) {
    while (true) {
        // function to connect to another peer
        // buffer for the message
        char buffer [BUFFER];
        sockaddr_in senderAddr;
        socklen_t addrlen = sizeof(senderAddr);
        // zeroes out the senderAddr struct
        memset(&senderAddr, 0, addrlen);

        // recieve bytes
        int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (sockaddr*)&senderAddr, &addrlen);
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

        if (message.header.type == MessageType::ACK && self->getWaitingForAck()) {
            self->setWaitingForAck(false);
            logger->clearFile();
        }
        // if its a command send the command to the satellite
        else if (message.header.type == MessageType::COMMAND) self->handleCommand(static_cast<const Command&>(message));

        // add if not already known
        if (!handler->hasConnection(message.senderId)) handler->addConnection(message.senderPort, ip, message.senderId, satId);
        else handler->heartbeatSat(message.senderId);

        if (message.senderId != 0) queue->pushBack("[NETWORK] Message received from Satellite: " + std::to_string(message.senderId));
        else queue->pushBack("[NETWORK] Message received from Ground Control");
    }
}