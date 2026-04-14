/*
File: ConnectionHandler
Date Created: March 30th, 2026
Last Updated: April 9th, 2026
Author: Tate Smith
Purpose: This file represents the handler for managing all peer connections in the network, it can add and remove connections, 
send messages to specific peers, and broadcast messages to all peers
*/

#include "ConnectionHandler.h"
#include <iostream>

ConnectionHandler::ConnectionHandler(MessageQueue *queue) : queue(queue) {}

void ConnectionHandler::addIncomingConnection(int port, const std::string& ip, int peerId, int satId) {
    // create a new connection and add it to the map
    auto [it, inserted] = connections.emplace(peerId, PeerConnection(peerId, ip, port, queue, satId));
    if (inserted) {
        it->second.heartbeat();
        it->second.markConnected();
    }
}

void ConnectionHandler::addOutgoingConnection(int port, const std::string& ip, int peerId, int satId) {
    // create a new connection and add it to the map, and connect to it if it works
    auto [it, inserted] = connections.emplace(peerId, PeerConnection(peerId, ip, port, queue, satId));
    if (inserted) {
        it->second.setOutgoing(true);
        it->second.connect();
    }
}

void ConnectionHandler::update() {
    // loop through every connection in the map, and if they are disconnected and outgoing try to reconnect
    for (auto& i : connections) {
        // check if peers are disconnected
        if (i.second.getState() == ConnectionState::CONNECTED && i.second.isTimedOut()) {
            // push message to logger queue
            queue->pushBack("Satellite Id: " + std::to_string(i.first) + " has timed out");
            i.second.disconnect();
        }

        // try to reconnect if disconnected
        else if (i.second.getState() == ConnectionState::DISCONNECTED && i.second.getOutgoing() && i.first != 0) {
            i.second.reconnect();
        }
    }
}

void ConnectionHandler::removeConnection(int peerId) {
    // remove a connection based off of its id
    connections.erase(peerId);
}

PeerConnection* ConnectionHandler::getConnection(int peerId) {
    // get a connection at a specific id
    auto it = connections.find(peerId);
    if (it == connections.end()) return nullptr;
    return &it->second;
}

void ConnectionHandler::sendMessageToPeer(int peerId, const Message& message) {
    // send a message to a specific peer
    auto peer = connections.find(peerId);
    if (peer == connections.end()) {
        // push message to logger queue
        queue->pushBack("Satellite Id: " + std::to_string(peerId) + " Not found");
        return;
    }
    peer->second.sendMessage(message);
}

void ConnectionHandler::broadcastMessage(const Message& message) {
    // loop through all peers and send them a message
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
    for (auto& i : connections) {
        if (i.first != 0) {
            std::string state;
            switch(i.second.getState()) {
                case CONNECTED: state = "CONNECTED"; break;
                case DISCONNECTED: state = "DISCONNECTED"; break;
            }
            // push message to logger queue
            queue->pushBack("Satellite Id " + std::to_string(i.first) + ": " + state);
        }
        else {
            switch(i.second.getState()) {
                case CONNECTED: queue->pushBack("Connected to Ground"); break;
                case DISCONNECTED: queue->pushBack("Disconnected from Ground"); break;
            }
        }
    }
}