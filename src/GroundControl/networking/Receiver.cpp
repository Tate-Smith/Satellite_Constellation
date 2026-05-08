/*
File: Reciever
Date Created: April 9th, 2026
Last Updated: May 8th, 2026
Author: Tate Smith
Purpose: This file is the listener for the ground control station, it starts a server to listen from,
then will listen for incoming messages and handles them
*/

#include "Receiver.h"

Receiver::Receiver(const int gcPort, MessageQueue<std::string> *loggerQueue, MessageQueue<SatOutput> *outputQueue, std::atomic<bool> *running) : 
serverSocket(-1), loggerQueue(loggerQueue), outputQueue(outputQueue), running(running), gcPort(gcPort) {}

void Receiver::handleFileDump(const File_Msg& msg) {
    /*
    This method takes in a file message struct and adds the message to a vector, if the message is the 
    last chunk of the batch it parses the vector and adds it to the output queue for the terminal to
    deal with
    */
    assert(msg.header.size > 0);
    assert(msg.senderId > 0);
    assert(msg.senderPort >= 0);
    assert(outputQueue != nullptr);
    auto& chunk = this->buffer[msg.senderId];
    chunk.insert(chunk.end(), msg.data, msg.data + msg.len);

    // if it is the last chunk of data
    if (msg.last) {
        // parse this chunk and then add it to the queue
        outputQueue->pushBack(parser.dataDecoder(msg.senderId, chunk, true));
        chunk.clear();
    }
}

void Receiver::addConnection(Message &message, GCConnectionHandler *handler, char ip[]) {
    /*
    This method takes in a message references, a connection handler pointer and a char array ip,
    it checks if this connection already exists in the handler, if not it adds the connection,
    and logs that a message was received from that satellite
    */
    assert(message.header.size > 0);
    assert(message.senderId > 0);
    assert(message.senderPort >= 0);
    assert(ip);
    assert(handler != nullptr);
    assert(gcPort >= 0);
    assert(outputQueue != nullptr);
    assert(loggerQueue != nullptr);
    // add if not already known
    if (!handler->hasConnection(message.senderId)) {
        handler->addConnection(message.senderPort, ip, message.senderId);
        handler->heartbeatSat(message.senderId);
    }
    else {
        // if known update connected status and heartbeat
        handler->heartbeatSat(message.senderId);
        outputQueue->pushBack(parser.dataDecoder(message.senderId, emptyChunk, true));
    }

    loggerQueue->pushBack("[NETWORK] Message received from Satellite Id: " + std::to_string(message.senderId));
}

void Receiver::ack(Message &message, GCConnectionHandler *handler) {
    /*
    This method takes in a message reference and a pointer to a connection handler, it then sends an
    ack to the satellite to let it know it received its message once the message is roperly handled
    */
    assert(message.header.size > 0);
    assert(message.senderId > 0);
    assert(message.senderPort >= 0);
    assert(handler != nullptr);
    assert(gcPort >= 0);
    assert(loggerQueue != nullptr);
    // keep track of whether a message was properly handled or not
    bool handled = false;
    // decide what to do with the message
    if (message.header.type == MessageType::FILE_MSG) {
        // if it is a file dump
        assert(message.header.size == sizeof(File_Msg));
        handleFileDump(static_cast<const File_Msg&>(message));
        handled = true;
    }
    else if (message.header.type == MessageType::HEARTBEAT) {
        // if it is a heartbeat
        handled = true;
    }
    else if (message.header.type == MessageType::ACK) {
        // if an ack from recieving a command
        loggerQueue->pushBack("[NETWORK] Satellite: " + std::to_string(message.senderId) + " acknowledged command");
        handled = true;
    }

    // send an ack message to the given satellite let it know the message was received if the message was properly handled
    if (handled) {
        Ack m;
        m.header.size = sizeof(m);
        m.header.type = MessageType::ACK;
        m.senderId = 0;
        m.senderPort = gcPort;
        m.received = true;
        handler->sendMessageToSat(message.senderId, m);
    }
}

void Receiver::startServer() {
    /*
    This method starts a server to begin listening on a given port
    */
    assert(gcPort >= 0);
    assert(loggerQueue != nullptr);
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
    serverAddr.sin_port = htons(gcPort);
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

    loggerQueue->pushBack("[NETWORK] Server started on port: " + std::to_string(gcPort));
}   

void Receiver::listen(GCConnectionHandler *handler) {
    /*
    This method takes in a connection handler pointer, and continuously loops until it receives a
    message it then handles that method and adds it to the connections in connection handler if
    not already present. It is run by the listener thread 
    */
    assert(Config::BUFFER_SIZE > 0);
    assert(handler != nullptr);
    // buffer for the message
    char buf [Config::BUFFER_SIZE];
    std::unique_ptr<Message> msg;
    while (running->load()) {
        // function to connect to another peer
        sockaddr_in senderAddr;
        socklen_t addrlen = sizeof(senderAddr);
        // zeroes out the senderAddr struct
        memset(&senderAddr, 0, addrlen);

        // recieve bytes
        int bytesReceived = recvfrom(this->serverSocket, buf, sizeof(buf), 0, (sockaddr*)&senderAddr, &addrlen);
        if (bytesReceived < 0) continue;

        // get the sender ip, and create or find a connection
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &senderAddr.sin_addr, ip, sizeof(ip));

        // convert the bytes to a message
        try {
            msg = decode(reinterpret_cast<uint8_t*>(buf), bytesReceived);
        }
        catch (...) {
            // skip the udp packet if it isnt decoded properly
            continue;
        }

        Message& message = *msg;

        addConnection(message, handler, ip);
        ack(message, handler);
    }
}