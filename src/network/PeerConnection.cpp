/*
File: PeerConnection
Date Created: March 30th, 2026
Last Updated: April 29th, 2026
Author: Tate Smith
Purpose: This file represents a connection to a peer in the network, and it can send and receive messages, and manage the connection
*/

#include "PeerConnection.h"

PeerConnection::PeerConnection(int id, const std::string& ip, int port, MessageQueue *queue, int satId, int listeningPort) : 
peerId(id), satId(satId), peerIp(ip), peerPort(port), peerSocket(-1), state(DISCONNECTED), 
lastHeartbeat(time(nullptr)), lastReconnect(time(nullptr)), retryCounter(0), listeningPort(listeningPort), queue(queue) {}

void PeerConnection::connect() {
    // this function connects to a peer, it create a socket and sets its state accordingly based on whether it connects
    this->peerSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (peerSocket < 0) {
        queue->pushBack("Error creating socket for peer: " + std::to_string(peerId));
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
        queue->pushBack("Invalid address / Address not supported for peer: " + std::to_string(peerId));
        return;
    }
    // whenever it attempts to connect it needs to send a message to the peer first to establish a conenction
    Heartbeat m{};
    m.header.type = MessageType::HEARTBEAT;
    m.header.size = sizeof(m);
    m.senderId = satId;
    m.senderPort = this->listeningPort;
    // get current time stamp
    m.timestamp = time(nullptr);
    m.alive = true;
    PeerConnection::sendMessage(m);
    queue->pushBack("Connecting to Satellite at address: " + peerIp + ":" + std::to_string(peerPort));
    this->state = ConnectionState::CONNECTING;
}

void PeerConnection::disconnect() {
    // this function disconnects from a peer, as long as the socket hasn't been closed already
    if (this->peerSocket >= 0) {
        close(this->peerSocket);
        this->peerSocket = -1;
    }

    this->state = ConnectionState::DISCONNECTED;
    this->lastReconnect = time(nullptr);
}

void PeerConnection::sendMessage(const Message& message) {
    // function to send a message to another peer, sends the specified message to a neighbor, if it was negative then there was an error
    // serialize the message into a byte
    std::vector<std::uint8_t> msg = message.serialize();
    int sent = sendto(this->peerSocket, reinterpret_cast<const char*>(msg.data()), 
    static_cast<int>(msg.size()), 0, (sockaddr*)&peerAddr, sizeof(peerAddr));
    if (sent < 0) {
        queue->pushBack("Error sending message to: "  + std::to_string(peerId));
    } else {
        if (peerId != 0) queue->pushBack("Sent message to Satellite Id: " + std::to_string(peerId));
        else queue->pushBack("Sent message to Ground Control");
    }
}

void PeerConnection::heartbeat() {
    // updates the last heartbeat detected
    this->lastHeartbeat = time(nullptr);
}

void PeerConnection::reconnect() {
    // try to reestablish a connection with the peer
    // get current time
    time_t curTime = time(nullptr);
    // check if the last reconnect attempt was over 10 seconds ago
    if (curTime - this->lastReconnect < 10) return;
    this->retryCounter++;
    // if over 10 reconnect attempts skip ie peer is dead
    if (this->retryCounter > 10) return;
    this->lastReconnect = curTime;
    queue->pushBack("Reconnecting to Satellite Id: " + std::to_string(this->peerId) + "; Reconnect counter = " + std::to_string(this->retryCounter));
    // clean up first
    PeerConnection::disconnect();
    PeerConnection::connect();
}

ConnectionState PeerConnection::getState() {
    // return the clients current state
    return this->state;
}

bool PeerConnection::isTimedOut() const {
    return (time(nullptr) - lastHeartbeat) > 11;
}

void PeerConnection::markConnected() {
    this->state = ConnectionState::CONNECTED;
    this->retryCounter = 0;
}