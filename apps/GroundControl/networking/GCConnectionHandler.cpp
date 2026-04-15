#include "GCConnectionHandler.h"
#include <iostream>

void GCConnectionHandler::addIncomingConnection(int port, const std::string& ip, int satId) {
    // create a new connection and add it to the map
    auto [it, inserted] = satellites.emplace(satId, Connection(satId, port, ip));
    if (inserted) {
        it->second.heartbeat();
    }
}

void GCConnectionHandler::addOutgoingConnection(int port, const std::string& ip, int satId) {
    // create a new connection and add it to the map, and connect to it if it works
    auto [it, inserted] = satellites.emplace(satId, Connection(satId, port, ip));
    if (inserted) {
        it->second.connect();
    }
}

void GCConnectionHandler::update() {
    // loop through every connection in the map, and if they are disconnected and outgoing try to reconnect
    for (auto& i : satellites) {
        // check if satellites are disconnected
        if (i.second.getState() == GCConnectionState::CONNECTED && i.second.isTimedOut()) {
            // push message to logger queue
            std::cout << "Satellite Id: " << i.first << " has timed out" << std::endl;
            i.second.disconnect();
        }

        // try to reconnect if disconnected
        else if (i.second.getState() == GCConnectionState::DISCONNECTED && i.first != 0) {
            i.second.reconnect();
        }
    }
}

void GCConnectionHandler::removeConnection(int satId) {
    // remove a connection based off of its id
    satellites.erase(satId);
}

Connection* GCConnectionHandler::getConnection(int satId) {
    // get a connection at a specific id
    auto it = satellites.find(satId);
    if (it == satellites.end()) return nullptr;
    return &it->second;
}

void GCConnectionHandler::sendMessageToSat(int satId, const Message& message) {
    // send a message to a specific satellite
    auto peer = satellites.find(satId);
    if (peer == satellites.end()) {
        // push message to logger queue
        std::cout << "Satellite Id: " << satId << " Not found" << std::endl;
        return;
    }
    peer->second.sendMessage(message);
}

void GCConnectionHandler::broadcastMessage(const Message& message) {
    // loop through all Satellites and send them a message
    for (auto& i : satellites) {
        if (i.second.getState() == GCConnectionState::CONNECTED) {
            i.second.sendMessage(message);
        }
        else {
            // push message to logger queue
            std::cout << "Satellite Id: " << i.first << " Not connected, skipping message" << std::endl;
        }
    }
}

void GCConnectionHandler::printAllSats() {
    for (auto& i : satellites) {
        std::string state;
        switch(i.second.getState()) {
            case CONNECTED: state = "CONNECTED"; break;
            case DISCONNECTED: state = "DISCONNECTED"; break;
        }
        // push message to logger queue
        std::cout << "Satellite Id " << i.first << ": " + state << std::endl;
    }
}