/*
File: ConnectionHandler
Date Created: March 30th, 2026
Last Updated: May 7th, 2026
Author: Tate Smith
Purpose: This file represents the handler for managing all peer connections in the network, it can add and remove connections, 
send messages to specific peers, and broadcast messages to all peers
*/

#include "ConnectionHandler.h"

ConnectionHandler::ConnectionHandler(MessageQueue<std::string> *loggerQueue, int listeningPort) : loggerQueue(loggerQueue), listeningPort(listeningPort) {
    assert(loggerQueue != nullptr);
    assert(listeningPort >= 0);
}

void ConnectionHandler::addConnection(int port, const std::string& ip, int peerId, int satId) { 
    /*
    This method takes in an int port, a string reference for the ip, an int for the peerId and one for this own satellites id,
    it then creates a connection to that peer and adds it to the map
    */
    assert(port >= 0);
    assert(!ip.empty());
    assert(peerId >= 0);
    assert(satId > 0);
    assert(loggerQueue != nullptr);
    // create a new connection and add it to the map
    std::lock_guard<std::mutex> lock(this->mtx);
    auto [it, inserted] = connections.emplace(peerId, PeerConnection(peerId, ip, port, loggerQueue, satId, this->listeningPort));
    if (inserted) {
        PeerConnection &p = it->second;
        p.heartbeat();
        p.connect();
        p.markConnected();
    }
}

void ConnectionHandler::update() {
    /*
    This method checks to see if peers are connected if not it attempts to reconnect
    */
    assert(loggerQueue != nullptr);
    // loop through every connection in the map, and if they are disconnected try to reconnect
    std::lock_guard<std::mutex> lock(this->mtx);
    for (auto& i : connections) {
        // check if peers are disconnected
        if (i.second.getState() == ConnectionState::CONNECTED && i.second.isTimedOut()) {
            // push message to logger queue
            loggerQueue->pushBack("[ERROR] Satellite: " + std::to_string(i.first) + " has timed out");
            i.second.disconnect();
        }

        // try to reconnect if disconnected
        else if ((i.second.getState() == ConnectionState::DISCONNECTED || i.second.getState() == ConnectionState::CONNECTING)) {
            i.second.reconnect();
        }
    }
}

void ConnectionHandler::removeConnection(int peerId) {
    /*
    This method takes in a peerId and removes that connection from the map
    */
    assert(peerId >= 0);
    assert(loggerQueue != nullptr);
    // remove a connection based off of its id
    std::lock_guard<std::mutex> lock(this->mtx);
    connections.erase(peerId);
}

bool ConnectionHandler::hasConnection(int peerId) {
    /*
    This method takes in a peerId and checks if that connection exists in the map
    */
    assert(peerId >= 0);
    std::lock_guard<std::mutex> lock(this->mtx);
    return connections.find(peerId) != connections.end();
}

void ConnectionHandler::heartbeatSat(int peerId) {
    /*
    This method takes in a peerId and gets the connection from the map and updates its heartbeat and connection status
    */
    assert(peerId >= 0);
    assert(loggerQueue != nullptr);
    std::lock_guard<std::mutex>lock(this->mtx);
    auto it = connections.find(peerId);
    if (it != connections.end()) {
        it->second.heartbeat();
        it->second.markConnected();
    }
}

void ConnectionHandler::sendMessageToPeer(int peerId, const Message& message) {
    /*
    This method takes in a peerId and a message reference and sends that message to that peerId
    */
    assert(peerId >= 0);
    assert(message.header.size > 0);
    assert(message.senderId >= 0);
    assert(message.senderPort >= 0);
    assert(loggerQueue != nullptr);
    // send a message to a specific peer
    std::lock_guard<std::mutex> lock(this->mtx);
    auto peer = connections.find(peerId);
    if (peer == connections.end()) {
        // push message to logger queue
        loggerQueue->pushBack("[ERROR] Satellite: " + std::to_string(peerId) + " Not found");
        return;
    }
    // make sure the peer is connected as well
    if (peer->second.getState() == ConnectionState::CONNECTED) {
        peer->second.sendMessage(message);
    }
}

void ConnectionHandler::broadcastMessage(const Message& message) {
    /*
    This method takes in a message reference and sends it to all peers in the peer map
    */
    assert(message.header.size > 0);
    assert(message.senderId >= 0);
    assert(message.senderPort >= 0);
    assert(loggerQueue != nullptr);
    // loop through all peers and send them a message
    std::lock_guard<std::mutex> lock(this->mtx);
    for (auto& i : connections) {
        // if its not the ground control
        if (i.first != 0) {
            if (i.second.getState() == ConnectionState::CONNECTED) {
                i.second.sendMessage(message);
            }
            else {
                // push message to logger queue
                loggerQueue->pushBack("[ERROR] Satellite: " + std::to_string(i.first) + " Not connected, skipping message");
            }
        }
    }
}

void ConnectionHandler::printAllPeers() {
    /*
    This method loops through all peers in the map and lists their current connection state
    */
    assert(loggerQueue != nullptr);
    std::lock_guard<std::mutex> lock(this->mtx);
    for (auto& i : connections) {
        if (i.first != 0) {
            const char *state = "DISCONNECTED";
            if (i.second.getState() == ConnectionState::CONNECTED) state = "CONNECTED";
            else if (i.second.getState() == ConnectionState::CONNECTING) state = "CONNECTING";
            // push message to logger queue
            loggerQueue->pushBack("[STATE] Satellite: " + std::to_string(i.first) + ": " + state);
        }
        else {
            switch(i.second.getState()) {
                case CONNECTED: loggerQueue->pushBack("[STATE] Connected to Ground"); break;
                case CONNECTING: loggerQueue->pushBack("[STATE] Connecting to Ground"); break;
                case DISCONNECTED: loggerQueue->pushBack("[ERROR] Disconnected from Ground"); break;
            }
        }
    }
}