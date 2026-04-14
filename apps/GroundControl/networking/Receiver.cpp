#include "Receiver.h"
#include <cstring>
#include <iostream>
#include "../../../src/protocol/Message.h"
#include "../../../src/protocol/Serializer.h"
#include <arpa/inet.h>

int PORT = 8000; // always listening on port 8000

void Receiver::startServer() {
    // function to start a server on the specified port
    // creates an IPv4 UDP socket and returns a file descriptor for the socket
    this->serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    // if the file descriptor is negative then there was an error creating the socket
    if (serverSocket < 0) {
        std::cout << "Error creating socket" << std::endl;
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
        std::cout << "Error binding socket" << std::endl;
        return;
    }
    // set a timeout to prevent blocking
    struct timeval timeout;
    timeout.tv_sec = 0; 
    timeout.tv_usec = 100000; // 100ms
    setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    std::cout << "Server started on port: " << PORT << std::endl;
}   

void Receiver::listen() {
    while (true) {
        // function to connect to another peer
        // buffer for the message
        char buffer [sizeof(Message)];
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
        Message message = deserializeMessage(std::vector<uint8_t>(buffer, buffer + bytesReceived));

        std::cout << "Message received from Satellite Id: " << message.senderId << std::endl;

        // send an ack message back to the satellite to let it know that ground control recieved it
        Ack m{};
        m.senderId = 0;
        m.type = MessageType::ACK;
        m.received = true;

        // function to send messages to the satellite
        std::vector<std::uint8_t> msg = serializeMessage(message);
        int sent = sendto(ntohs(senderAddr.sin_port), reinterpret_cast<const char*>(msg.data()), 
        static_cast<int>(msg.size()), 0, (sockaddr*)&senderAddr, sizeof(senderAddr));
        // check if there was an error sending the message
        if (sent < 0) {
            std::cout << "Error sending message" << std::endl;
        } else {
            std::cout <<  "Sent message to Satellite" << std::endl;
        }
    }
}