/*
File: ConnectionHandler
Date Created: March 30th, 2026
Last Updated: April 28th, 2026
Author: Tate Smith
Purpose: This file represents the handler for managing all peer connections in the network, it can add and remove connections, 
send messages to specific peers, and broadcast messages to all peers
*/

#include "ConnectionHandler.h"

ConnectionHandler::ConnectionHandler(MessageQueue *queue, int listeningPort) : queue(queue), listeningPort(listeningPort) {}

void ConnectionHandler::addConnection(int port, const std::string& ip, int peerId, int satId) {
    // create a new connection and add it to the map
    std::lock_guard<std::mutex> lock(this->mtx);
    auto [it, inserted] = connections.emplace(peerId, PeerConnection(peerId, ip, port, queue, satId, this->listeningPort));
    if (inserted) {
        it->second.heartbeat();
        it->second.connect();
        it->second.markConnected();
    }
}

void ConnectionHandler::update() {
    // loop through every connection in the map, and if they are disconnected and outgoing try to reconnect
    std::lock_guard<std::mutex> lock(this->mtx);
    for (auto& i : connections) {
        // check if peers are disconnected
        if (i.second.getState() == ConnectionState::CONNECTED && i.second.isTimedOut()) {
            // push message to logger queue
            queue->pushBack("Satellite Id: " + std::to_string(i.first) + " has timed out");
            i.second.disconnect();
        }

        // try to reconnect if disconnected
        else if ((i.second.getState() == ConnectionState::DISCONNECTED || i.second.getState() == ConnectionState::CONNECTING)) {
            i.second.reconnect();
        }
    }
}

void ConnectionHandler::removeConnection(int peerId) {
    // remove a connection based off of its id
    std::lock_guard<std::mutex> lock(this->mtx);
    connections.erase(peerId);
}

bool ConnectionHandler::hasConnection(int satId) {
    std::lock_guard<std::mutex> lock(this->mtx);
    return connections.find(satId) != connections.end();
}

void ConnectionHandler::heartbeatSat(int satId) {
    std::lock_guard<std::mutex>lock(this->mtx);
    auto it = connections.find(satId);
    if (it != connections.end()) {
        it->second.heartbeat();
        it->second.markConnected();
    }
}

void ConnectionHandler::sendMessageToPeer(int peerId, const Message& message) {
    // send a message to a specific peer
    std::lock_guard<std::mutex> lock(this->mtx);
    auto peer = connections.find(peerId);
    if (peer == connections.end()) {
        // push message to logger queue
        queue->pushBack("Satellite Id: " + std::to_string(peerId) + " Not found");
        return;
    }
    // make sure the peer is connected as well
    if (peer->second.getState() == ConnectionState::CONNECTED) {
        peer->second.sendMessage(message);
    }
}

void ConnectionHandler::broadcastMessage(const Message& message) {
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
                queue->pushBack("Satellite Id: " + std::to_string(i.first) + " Not connected, skipping message");
            }
        }
    }
}

void ConnectionHandler::printAllPeers() {
    std::lock_guard<std::mutex> lock(this->mtx);
    for (auto& i : connections) {
        if (i.first != 0) {
            std::string state;
            switch(i.second.getState()) {
                case CONNECTED: state = "CONNECTED"; break;
                case CONNECTING: state = "CONNECTING"; break;
                case DISCONNECTED: state = "DISCONNECTED"; break;
            }
            // push message to logger queue
            queue->pushBack("Satellite Id " + std::to_string(i.first) + ": " + state);
        }
        else {
            switch(i.second.getState()) {
                case CONNECTED: queue->pushBack("Connected to Ground"); break;
                case CONNECTING: queue->pushBack("Connecting to Ground"); break;
                case DISCONNECTED: queue->pushBack("Disconnected from Ground"); break;
            }
        }
    }
}