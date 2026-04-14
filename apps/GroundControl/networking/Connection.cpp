/*
File: Connection
Date Created: April 9th, 2026
Last Updated: April 9th, 2026
Author: Tate Smith
Purpose: This file handles connecting to a satellite and sending messages to it
*/

#include "Connection.h"
#include <sys/socket.h>
#include <iostream>
#include <sys/_endian.h>
#include <Kernel/sys/_endian.h>
#include <arpa/inet.h>

Connection::Connection(int id, int port, std::string ip) : satSocket(-1), port(port), ip(ip) {}

void Connection::connect() {
    // function to connect to a satellite
    this->satSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (satSocket < 0) {
        std::cout << "Error creating socket for peer" << std::endl;
        return;
    }

    // zeroes out the peerAddr struct
    memset(&peerAddr, 0, sizeof(peerAddr));
    // sets the port in network byte order
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(port);

    // converts the ip into binary, and if its negative then there was an error
    if (inet_pton(AF_INET, ip.c_str(), &peerAddr.sin_addr) <= 0) {
        std::cout << "Invalid address / Address not supported for peer" << std::endl;
        return;
    }

    // whenever it attempts to connect it needs to send a message to the satellite first to establish a connection
    Heartbeat m;
    m.type = MessageType::HEARTBEAT;
    m.senderId = 0;
    // get current time stamp
    m.timestamp = time(nullptr);
    m.alive = true;
    Connection::sendMessage(m);
    std::cout << "Connecting to Satellite at address: " << ip << ":" << std::to_string(port) << std::endl;
}

void Connection::sendMessage(const Message &message) {
    // function to send messages to the satellite
    std::vector<std::uint8_t> msg = serializeMessage(message);
    int sent = sendto(this->satSocket, reinterpret_cast<const char*>(msg.data()), 
    static_cast<int>(msg.size()), 0, (sockaddr*)&peerAddr, sizeof(peerAddr));
    // check if there was an error sending the message
    if (sent < 0) {
        std::cout << "Error sending message" << std::endl;
    } else {
        std::cout <<  "Sent message to Satellite" << std::endl;
    }
}

GCConnectionState Connection::getState() {
    // return the clients current state
    return this->state;
}

void Connection::disconnect(){

}

void Connection::heartbeat() {
    // updates the last heartbeat detected
    this->lastHeartbeat = time(nullptr);
}

void Connection::reconnect() {

}

bool Connection::isTimedOut() const {
    return (time(nullptr) - lastHeartbeat) > 8;
}

void Connection::markConnected() {
    this->state = ConnectionState::CONNECTED;
}
