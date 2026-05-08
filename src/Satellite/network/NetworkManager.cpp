/*
File: NetworkManager
Date Created: March 28th, 2026
Last Updated: May 8th, 2026
Purpose: This file contains the implementation for the NetworkManager class, which is responsible for handling all network listening
It can start a server, and listen for messages on that port, as well as keeping the connections up to date
*/

#include "NetworkManager.h"

NetworkManager::NetworkManager(MessageQueue<std::string> *loggerQueue, Satellite *self, Logger *logger, int satId, std::atomic<bool> *running) : 
serverSocket(-1), satId(satId), loggerQueue(loggerQueue), self(self), logger(logger), running(running) {
    assert(loggerQueue != nullptr);
    assert(self != nullptr);
    assert(logger != nullptr);
    assert(running != nullptr);
    assert(satId > 0);
}

void NetworkManager::ack(ConnectionHandler *handler) {
    /*
    This method takes in a connection handler pointer, and sends an ack back to the ground control
    for when it recieves a command from the ground control and processes it correctly
    */
    assert(handler != nullptr);
    Ack a;
    a.header.size = sizeof(a);
    a.header.type = MessageType::ACK;
    a.senderId = satId;
    a.senderPort = self->getPort();
    a.received = true;
    handler->sendMessageToPeer(0, a);
}

void NetworkManager::startServer(int port) {
    /*
    This method takes in an int value port and starts a server on that port
    */
    assert(port > 0);
    assert(loggerQueue != nullptr);
    // function to start a server on the specified port
    // creates an IPv4 UDP socket and returns a file descriptor for the socket
    this->serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    // if the file descriptor is negative then there was an error creating the socket
    if (serverSocket < 0) {
        loggerQueue->pushBack("[ERROR] Error creating socket");
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
        loggerQueue->pushBack("[ERROR] Error binding socket");
        return;
    }
    // set a timeout to prevent blocking
    struct timeval timeout;
    timeout.tv_sec = 0; 
    timeout.tv_usec = Config::TIMEOUT_MS;
    int retval = setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    if (retval != 0) throw std::runtime_error("setsockopt failed");

    loggerQueue->pushBack("[NETWORK] Server started on port: " + std::to_string(port));
}   

void NetworkManager::acceptConnections(ConnectionHandler *handler) {
    /*
    This method takes in a connection handler pointer and listens for messages, when it receives one, it checks if
    that connection is already present in the handler, if not it adds the connection. It also can handle different 
    message types
    */
    assert(Config::BUFFER_SIZE > 0);
    assert(handler != nullptr);
    assert(loggerQueue != nullptr);
    assert(running != nullptr);
    // buffer for the message
    char buffer [Config::BUFFER_SIZE];
    std::unique_ptr<Message> msg;
    while (running->load()) {
        // function to connect to another peer
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
        else if (message.header.type == MessageType::COMMAND) {
            assert(message.header.size == sizeof(Command));
            self->handleCommand(static_cast<const Command&>(message));
            // send an ack back to the ground control to let it know that it was received
            ack(handler);
        }

        // add if not already known
        if (!handler->hasConnection(message.senderId)) handler->addConnection(message.senderPort, ip, message.senderId, satId);
        else handler->heartbeatSat(message.senderId);

        if (message.senderId != 0) loggerQueue->pushBack("[NETWORK] Message received from Satellite: " + std::to_string(message.senderId));
        else loggerQueue->pushBack("[NETWORK] Message received from Ground Control");
    }
}