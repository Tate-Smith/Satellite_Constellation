/*
File: Connection
Date Created: April 9th, 2026
Last Updated: May 8th, 2026
Author: Tate Smith
Purpose: This file handles the connection to a satellite, and methods to communicate with it
*/

#include "Connection.h"

Connection::Connection(int id, int port, const std::string &ip, int gcPort, MessageQueue<std::string> *loggerQueue) : id(id), satSocket(-1), port(port), ip(ip), 
state(DISCONNECTED), lastHeartbeat(time(nullptr)), lastReconnect(time(nullptr)), retryCounter(0), gcPort(gcPort), loggerQueue(loggerQueue) {}

void Connection::connect() {
    /*
    This method deals with setting up a connection between a satellite and the ground control,
    it sends a message to the satellite when attempting to connect
    */
    assert(port > 0);
    assert(!ip.empty());
    assert(loggerQueue != nullptr);
    // function to connect to a satellite
    this->satSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (satSocket < 0) {
        loggerQueue->pushBack("[ERROR] Error creating socket for peer");
        return;
    }

    // zeroes out the peerAddr struct
    memset(&peerAddr, 0, sizeof(peerAddr));
    // sets the port in network byte order
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(port);

    // converts the ip into binary, and if its negative then there was an error
    if (inet_pton(AF_INET, ip.c_str(), &peerAddr.sin_addr) <= 0) {
        loggerQueue->pushBack("[ERROR] Invalid address / Address not supported for peer");
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
    loggerQueue->pushBack("[NETWORK] Attempting to connect to Satellite " + std::to_string(this->id) + ", at address: " + ip + ":" + std::to_string(port));
}

void Connection::sendMessage(const Message &message) {
    /*
    This method deals with sending a message to a satellite, it provides the message to be sent
    it serializes the message first into bytes, then sends it
    */
    assert(satSocket >= 0);
    assert(message.senderPort > 0);
    assert(message.header.size > 0);
    assert(loggerQueue != nullptr);
    // function to send messages to the satellite
    this->msg = message.serialize();
    int sent = sendto(this->satSocket, reinterpret_cast<const char*>(msg.data()), 
    static_cast<int>(msg.size()), 0, (sockaddr*)&peerAddr, sizeof(peerAddr));
    // check if there was an error sending the message
    if (sent < 0) {
        loggerQueue->pushBack("[ERROR] Error sending message to Satellite");
    } 
    else if ((size_t) sent < msg.size()) {
        loggerQueue->pushBack("[ERROR] Partial send to Satellite");
    }
    else {
        loggerQueue->pushBack("[NETWORK] Sent message to Satellite");
    }
}

GCConnectionState Connection::getState() {
    /*
    This method returns the satellites current state
    */
    return this->state;
}

void Connection::disconnect(){
    /*
    This method disconnects from a satellite, as long as the socket hasn't been closed already
    */
    assert(lastReconnect >= 0);
    if (this->satSocket >= 0) {
        int retval = close(this->satSocket);
        if (retval <= 0) loggerQueue->pushBack("[ERROR] Socket failed to close");
        this->satSocket = -1;
    }

    this->state = GCConnectionState::DISCONNECTED;
    this->lastReconnect = time(nullptr);
}

void Connection::heartbeat() {
    /*
    This method updates the last heartbeat detected
    */
    assert(lastHeartbeat >= 0);
    this->lastHeartbeat = time(nullptr);
}

bool Connection::reconnect() {
    /*
    This method attempts to reconnect with a satellite that hasn't sent a message in a while, it attempts to recoonect every 10 seconds,
    if it has over 10 attempts it stops trying to reconnect and deems the satellite dead. It can still reconnect to the satellite
    however but the satellite must intiate the connection
    */
    assert(lastReconnect >= 0);
    assert(retryCounter >= 0);
    assert(loggerQueue != nullptr);
    // get current time
    time_t curTime = time(nullptr);
    // check if the last reconnect attempt was over the interval
    if (curTime - this->lastReconnect < Config::RECONNECT_INTERVAL) return true;
    this->retryCounter++;
    // if over the reconnect attempts reconnect attempts skip ie satellite is dead
    if (this->retryCounter > Config::MAX_RECONNECT_ATTEMPTS) return false;
    this->lastReconnect = curTime;
    loggerQueue->pushBack("[NETWORK] Reconnecting to Satellite Id: " + std::to_string(this->id) + "; Reconnect counter = " + std::to_string(this->retryCounter));
    // clean up first
    Connection::disconnect();
    Connection::connect();
    return true;
}

bool Connection::isTimedOut() const {
    /*
    This method determines of a satellite is timed out, based on if its last message was over 20 seconds ago
    */
    assert(lastHeartbeat >= 0);
    return (time(nullptr) - lastHeartbeat) > Config::TIMEOUT_THRESHOLD;
}

void Connection::markConnected() {
    /*
    This method marks a satellites connection as connected
    */
    assert(retryCounter >= 0);
    this->state = GCConnectionState::CONNECTED;
    this->retryCounter = 0;
}

int Connection::getId() {
    /*
    This method returns the id of a satellite
    */
    assert(id > 0);
    return this->id;
}

bool Connection::isDead() {
    /*
    This method returns if a satellite is dead or not, a satellite is considered dead if there have been over 10 reconnect attempts
    */
    return this->retryCounter > Config::MAX_RECONNECT_ATTEMPTS;
}