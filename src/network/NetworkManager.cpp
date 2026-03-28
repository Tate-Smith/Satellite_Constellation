#include "NetworkManager.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

void NetworkManager::startServer(int port) {
    // function to start a server on the specified port
    this->serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        return;
    }
}   

void NetworkManager::connectToPeer(std::string ip, int port) {
    // function to connect to another peer using the specified IP and port
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(port);
    peerAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    connect(serverSocket, (sockaddr*)&peerAddr, sizeof(peerAddr));
}   

void NetworkManager::sendMessage(std::string message) {
    // function to send a message to another peer using the specified IP and port
    sendto(serverSocket, message.c_str(), message.size(), 0, (sockaddr*)&peerAddr, sizeof(peerAddr));
}

void NetworkManager::receiveMessage() {
    // function to receive messages from other peers
    char buffer[4096];
    socklen_t addrLen = sizeof(peerAddr);
    int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (sockaddr*)&peerAddr, &addrLen);
    if (bytesReceived < 0) {
        std::cerr << "Error receiving message" << std::endl;
        return;
    }
    buffer[bytesReceived] = '\0';
    std::cout << "Received message: " << buffer << std::endl;

}