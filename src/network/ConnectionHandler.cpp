/*
File: ConnectionHandler
Date Created: March 30th, 2026
Last Updated: March 31st, 2026
Author: Tate Smith
Purpose: This file represents the handler for managing all peer connections in the network
*/

#include "ConnectionHandler.h"
#include <iostream>

void ConnectionHandler::addIncomingConnection(int port, const std::string& ip, int peerId) {
    // create a new connection and add it to the map
    connections.emplace(peerId, PeerConnection(peerId, ip, port));
}

void ConnectionHandler::addOutgoingConnection(int port, const std::string& ip, int peerId) {
    // create a new connection and add it to the map, and connect to it if it works
    auto [it, inserted] = connections.emplace(peerId, PeerConnection(peerId, ip, port));
    if (inserted) {
        it->second.setOutgoing(true);
        it->second.connect();
    }
}

void ConnectionHandler::update() {
    // loop through every connection in the map, and if they are disconnected and outgoing try to reconnect
    for (auto& i : connections) {
        if (i.second.getState() == ConnectionState::DISCONNECTED && i.second.getOutgoing()) {
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
        std::cout << "Peer: " << peerId << " Not found" << std::endl;
        return;
    }
    peer->second.sendMessage(message);
}

void ConnectionHandler::broadcastMessage(const Message& message) {
    // loop through all peers and send them a message
    for (auto& i : connections) {
        if (i.second.getState() == ConnectionState::CONNECTED) {
            i.second.sendMessage(message);
        }
    }
}