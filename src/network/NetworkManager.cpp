/*
File: NetworkManager
Date Created: March 28th, 2026
Last Updated: March 31st, 2026
Purpose: This file contains the implementation for the NetworkManager class, which is responsible for handling all network listening
It can start a server, and accept connections from other peers, and it uses the ConnectionHandler to manage the connections and messages
*/

#include "NetworkManager.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include "../protocol/Message.h"
#include "../protocol/Serializer.h"

void NetworkManager::startServer(int port, const MessageQueue& queue) {
    // function to start a server on the specified port
    // creates an IPv4 UDP socket and returns a file descriptor for the socket
    this->serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    // this->queue = queue;
    selector.addSocket(this->serverSocket);
    // if the file descriptor is negative then there was an error creating the socket
    if (serverSocket < 0) {
        std::cerr << "Error creating socket" << std::endl;
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
        std::cerr << "Error binding socket" << std::endl;
        return;
    }
    // set a timeout to prevent blocking
    struct timeval timeout;
    timeout.tv_sec = 0; 
    timeout.tv_usec = 100000; // 100ms
    setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    std::cout << "Server started on port: " << port << std::endl;
}   

void NetworkManager::acceptConnections() {
    // function to connect to another peer
    // buffer for the message
    char buffer [sizeof(Message)];
    sockaddr_in senderAddr;
    socklen_t addrlen = sizeof(senderAddr);
    // zeroes out the senderAddr struct
    memset(&senderAddr, 0, addrlen);

    // recieve bytes
    int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (sockaddr*)&senderAddr, &addrlen);
    if (bytesReceived < 0) return;

    // get the sender ip, and create or find a connection
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &senderAddr.sin_addr, ip, sizeof(ip));

    // convert the bytes to a message
    Message message = deserializeMessage(std::vector<uint8_t>(buffer, buffer + bytesReceived));

    // push message to the message queue
    queue.push(message);

    std::cout << "Message received from: " << message.senderId << std::endl;
}