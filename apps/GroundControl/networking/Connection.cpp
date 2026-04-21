/*
File: Connection
Date Created: April 9th, 2026
Last Updated: April 21st, 2026
Author: Tate Smith
Purpose: This file handles connecting to a satellite and sending messages to it
*/

#include "Connection.h"

Connection::Connection(int id, int port, std::string ip) : id(id), satSocket(-1), port(port), ip(ip), state(DISCONNECTED), 
lastHeartbeat(time(nullptr)), lastReconnect(time(nullptr)), retryCounter(0) {}

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
    m.senderId = 0;
    m.header.type = MessageType::HEARTBEAT;
    // get current time stamp
    m.timestamp = time(nullptr);
    m.alive = true;
    m.header.size = sizeof(m);
    Connection::sendMessage(m);
    std::cout << "Connecting to Satellite " << this->id << ", at address: " << ip << ":" << std::to_string(port) << std::endl;
}

void Connection::sendMessage(const Message &message) const {
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
    // return the satellites current state
    return this->state;
}

void Connection::disconnect(){
    // this function disconnects from a satellite, as long as the socket hasn't been closed already
    if (this->satSocket >= 0) {
        close(this->satSocket);
        this->satSocket = -1;
    }

    this->state = GCConnectionState::DISCONNECTED;
    this->lastReconnect = time(nullptr);
}

void Connection::heartbeat() {
    // updates the last heartbeat detected
    this->lastHeartbeat = time(nullptr);
}

void Connection::reconnect() {
    // try to reestablish a connection with the satellite
    // get current time
    time_t curTime = time(nullptr);
    // check if the last reconnect attempt was over 10 seconds ago
    if (curTime - this->lastReconnect < 10) return;
    this->retryCounter++;
    // if over 10 reconnect attempts skip ie satellite is dead
    if (this->retryCounter > 10) return;
    this->lastReconnect = curTime;
    std::cout << "Reconnecting to Satellite Id: " << this->id << "; Reconnect counter = " << this->retryCounter << std::endl;
    // clean up first
    Connection::disconnect();
    Connection::connect();
}

bool Connection::isTimedOut() const {
    return (time(nullptr) - lastHeartbeat) > 8;
}

void Connection::markConnected() {
    this->state = GCConnectionState::CONNECTED;
    this->retryCounter = 0;
}
