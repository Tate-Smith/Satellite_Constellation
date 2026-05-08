/*
File: PeerConnection
Date Created: March 30th, 2026
Last Updated: May 7th, 2026
Author: Tate Smith
Purpose: This file represents a connection to a satellite in the network, it can send messages, and manage the connection
to the given peer
*/

#include "PeerConnection.h"

PeerConnection::PeerConnection(int id, const std::string& ip, int port, MessageQueue<std::string> *loggerQueue, int satId, int listeningPort) : 
peerId(id), satId(satId), peerIp(ip), peerPort(port), peerSocket(-1), state(DISCONNECTED), lastHeartbeat(time(nullptr)), 
lastReconnect(time(nullptr)), retryCounter(0), listeningPort(listeningPort), loggerQueue(loggerQueue) {
    assert(id >= 0);
    assert(port >= 0);
    assert(loggerQueue != nullptr);
}

void PeerConnection::connect() {
    /*
    This method connects to a peer and sends a Heartbeat message
    */
    assert(this->peerId >= 0);
    assert(!this->peerIp.empty());
    assert(this->peerPort >= 0);
    assert(this->listeningPort >= 0);
    assert(loggerQueue != nullptr);
    // this function connects to a peer, it create a socket and sets its state accordingly based on whether it connects
    this->peerSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (peerSocket < 0) {
        loggerQueue->pushBack("[ERROR] Error creating socket for Satellite: " + std::to_string(peerId));
        return;
    }

    // set state to connecting
    this->lastHeartbeat = time(nullptr);

    // zeroes out the peerAddr struct
    memset(&peerAddr, 0, sizeof(peerAddr));
    // sets the port in network byte order
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(peerPort);

    // converts the ip into binary, and if its negative then there was an error
    if (inet_pton(AF_INET, peerIp.c_str(), &peerAddr.sin_addr) <= 0) {
        loggerQueue->pushBack("[ERROR] Invalid address / Address not supported for Satellite: " + std::to_string(peerId));
        return;
    }
    // whenever it attempts to connect it needs to send a message to the peer first to establish a connection
    Heartbeat m{};
    m.header.type = MessageType::HEARTBEAT;
    m.header.size = sizeof(m);
    m.senderId = satId;
    m.senderPort = this->listeningPort;
    m.alive = true;
    PeerConnection::sendMessage(m);
    loggerQueue->pushBack("[NETWORK] Connecting to Satellite at address: " + peerIp + ":" + std::to_string(peerPort));
    this->state = ConnectionState::CONNECTING;
}

void PeerConnection::disconnect() {
    /*
    This method disconnects from a peer, and sets its state and other variables accordingly
    */
    assert(loggerQueue != nullptr);
    if (this->peerSocket >= 0) {
        close(this->peerSocket);
        this->peerSocket = -1;
    }

    this->state = ConnectionState::DISCONNECTED;
    this->lastReconnect = time(nullptr);
}

void PeerConnection::sendMessage(const Message& message) {
    /*
    This method takes in a message reference and sends that message to this peer it represents
    */
    assert(message.header.size > 0);
    assert(message.senderId >= 0);
    assert(message.senderPort >= 0);
    assert(this->peerId >= 0);
    assert(loggerQueue != nullptr);
    // function to send a message to another peer, sends the specified message to a neighbor, if it was negative then there was an error
    // serialize the message into a byte
    this->msg = message.serialize();
    int sent = sendto(this->peerSocket, reinterpret_cast<const char*>(msg.data()), 
    static_cast<int>(msg.size()), 0, (sockaddr*)&peerAddr, sizeof(peerAddr));
    if (sent < 0) {
        loggerQueue->pushBack("[ERROR] Error sending message to Satellite: "  + std::to_string(peerId));
    } else {
        if (peerId != 0) loggerQueue->pushBack("[NETWORK] Sent message to Satellite: " + std::to_string(peerId));
        else loggerQueue->pushBack("[NETWORK] Sent message to Ground Control");
    }
}

void PeerConnection::heartbeat() {
    /*
    This method updates the last heartbeat time to now
    */
    assert(lastHeartbeat >= 0);
    this->lastHeartbeat = time(nullptr);
}

void PeerConnection::reconnect() {
    /*
    This method attempts to reconnect to the peer, if its been less than 10 seconds it will skip it, and if its been
    over 10 tries the satellite is considered dead and no longer attempts to connect to it
    */
    assert(loggerQueue != nullptr);
    assert(lastReconnect >= 0);
    // try to reestablish a connection with the peer
    // get current time
    time_t curTime = time(nullptr);
    // check if the last reconnect attempt was over 10 seconds ago
    if (curTime - this->lastReconnect < 10) return;
    this->retryCounter++;
    // if over 10 reconnect attempts skip ie peer is dead
    if (this->retryCounter > 10) return;
    this->lastReconnect = curTime;
    loggerQueue->pushBack("[ERROR] Reconnecting to Satellite: " + std::to_string(this->peerId) + "; Reconnect counter = " + std::to_string(this->retryCounter));
    // clean up first
    PeerConnection::disconnect();
    PeerConnection::connect();
}

ConnectionState PeerConnection::getState() {
    /*
    This method returns the satellites current connection state
    */
    assert(peerId >= 0);
    return this->state;
}

bool PeerConnection::isTimedOut() const {
    /*
    This method returns if the satellite hasn't had a hearbeat in over 20 seconds 
    */
    assert(lastHeartbeat >= 0);
    assert(state == ConnectionState::CONNECTED);
    return (time(nullptr) - lastHeartbeat) > 20;
}

void PeerConnection::markConnected() {
    /*
    This method marks the satellite as connected and resets the retry counter
    */
    assert(retryCounter >= 0);
    this->state = ConnectionState::CONNECTED;
    this->retryCounter = 0;
}