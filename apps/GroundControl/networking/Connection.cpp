/*
File: Connection
Date Created: April 9th, 2026
Last Updated: May 1st, 2026
Author: Tate Smith
Purpose: This file handles connecting to a satellite and sending messages to it
*/

#include "Connection.h"

Connection::Connection(int id, int port, std::string ip, int gcPort, MessageQueue<std::string> *queue) : id(id), satSocket(-1), port(port), ip(ip), 
state(DISCONNECTED), lastHeartbeat(time(nullptr)), lastReconnect(time(nullptr)), retryCounter(0), gcPort(gcPort), queue(queue) {}

void Connection::connect() {
    // function to connect to a satellite
    this->satSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (satSocket < 0) {
        queue->pushBack("Error creating socket for peer");
        return;
    }

    // zeroes out the peerAddr struct
    memset(&peerAddr, 0, sizeof(peerAddr));
    // sets the port in network byte order
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(port);

    // converts the ip into binary, and if its negative then there was an error
    if (inet_pton(AF_INET, ip.c_str(), &peerAddr.sin_addr) <= 0) {
        queue->pushBack("Invalid address / Address not supported for peer");
        return;
    }

    // whenever it attempts to connect it needs to send a message to the satellite first to establish a connection
    Heartbeat m;
    m.senderId = 0;
    m.senderPort = gcPort;
    m.header.type = MessageType::HEARTBEAT;
    m.alive = true;
    m.header.size = sizeof(m);
    Connection::sendMessage(m);
    queue->pushBack("Connecting to Satellite " + std::to_string(this->id) + ", at address: " + ip + ":" + std::to_string(port));
}

void Connection::sendMessage(const Message &message) const {
    // function to send messages to the satellite
    std::vector<std::uint8_t> msg = message.serialize();
    int sent = sendto(this->satSocket, reinterpret_cast<const char*>(msg.data()), 
    static_cast<int>(msg.size()), 0, (sockaddr*)&peerAddr, sizeof(peerAddr));
    // check if there was an error sending the message
    if (sent < 0) {
        queue->pushBack("Error sending message to Satellite");
    } else {
        queue->pushBack("Sent message to Satellite");
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

bool Connection::reconnect() {
    // try to reestablish a connection with the satellite
    // get current time
    time_t curTime = time(nullptr);
    // check if the last reconnect attempt was over 10 seconds ago
    if (curTime - this->lastReconnect < 10) return true;
    this->retryCounter++;
    // if over 10 reconnect attempts skip ie satellite is dead
    if (this->retryCounter > 10) return false;
    this->lastReconnect = curTime;
    queue->pushBack("Reconnecting to Satellite Id: " + std::to_string(this->id) + "; Reconnect counter = " + std::to_string(this->retryCounter));
    // clean up first
    Connection::disconnect();
    Connection::connect();
    return true;
}

bool Connection::isTimedOut() const {
    return (time(nullptr) - lastHeartbeat) > 11;
}

void Connection::markConnected() {
    this->state = GCConnectionState::CONNECTED;
    this->retryCounter = 0;
}

int Connection::getId() {
    return this->id;
}

bool Connection::isDead() {
    return this->retryCounter > 10;
}