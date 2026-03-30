/*
File: NetworkManager
Date Created: March 28th, 2026
Last Updated: March 30th, 2026
Purpose: This file contains the implementation for the NetworkManager class, which is responsible for handling all network communication 
between satellites. It can start a server, connect to peers, send messages, and receive messages.
*/

#include "NetworkManager.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include "../protocol/Message.h"
#include "../protocol/Serializer.h"

void NetworkManager::startServer(int port) {
    // function to start a server on the specified port
    // creates an IPv4 UDP socket and returns a file descriptor for the socket
    this->serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
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
    timeout.tv_sec = 1; // 1 second timeout
    timeout.tv_usec = 0;
    setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    std::cout << "Server started on port: " << port << std::endl;
}   

void NetworkManager::setPeer(std::string ip, int port) {
    // function to connect to another peer using the specified IP and port
    // zeroes out the peerAddr struct
    memset(&peerAddr, 0, sizeof(peerAddr));

    // sets the port in network byte order
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(port);
    // converts the ip into binary, and if its negative then there was an error
    if (inet_pton(AF_INET, ip.c_str(), &peerAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        return;
    }
    std::cout << "Connected to peer: " << ip << ":" << port << std::endl;
}   

void NetworkManager::sendMessage(const Message& message) {
    // function to send a message to another peer, sends the specified message to a neighbor, if it was negative then there was an error
    // serialize the message into a byte
    std::vector<std::uint8_t> msg = serializeMessage(message);
    int sent = sendto(serverSocket, reinterpret_cast<const char*>(msg.data()), static_cast<int>(msg.size()), 0, (sockaddr*)&peerAddr, sizeof(peerAddr));
    if (sent < 0) {
        std::cerr << "Error sending message" << std::endl;
    } else {
        std::cout << "Sent message to neighbor" << std::endl;
    }
}

void NetworkManager::receiveMessage() {
    // function to receive messages from other peers
    // buffer to store the incoming message, and sockaddr_in stores senders address
    char buffer[sizeof(Message)];
    sockaddr_in senderAddr;
    socklen_t addrLen = sizeof(senderAddr);
    // zeroes out the senderAddr struct
    memset(&senderAddr, 0, addrLen);

    // blocks until a message is recieved, it stores the message in buffer, if it is negative then there was an error
    int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (sockaddr*)&senderAddr, &addrLen);
    if (bytesReceived < 0) {
        std::cerr << "Error receiving message" << std::endl;
        return;
    }
    // deserialize the message and then print it
    Message message = deserializeMessage(std::vector<std::uint8_t>(buffer, buffer + bytesReceived));
    std::cout << "Received message: Satellite id: " << message.senderId 
    << ", Position: (" << message.x << ", " << message.y << ", " << message.z << ")" << std::endl;
}