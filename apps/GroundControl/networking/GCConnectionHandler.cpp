#include "GCConnectionHandler.h"

void GCConnectionHandler::addConnection(int port, const std::string& ip, int satId) {
    // create a new connection and add it to the map, and connect to it if it works
    std::lock_guard<std::mutex>lock(this->mtx);
    auto [it, inserted] = satellites.emplace(satId, Connection(satId, port, ip));
    if (inserted) {
        it->second.connect();
    }
}

void GCConnectionHandler::update() {
    // loop through every connection in the map, and if they are disconnected and outgoing try to reconnect
    std::lock_guard<std::mutex>lock(this->mtx);
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
    std::lock_guard<std::mutex>lock(this->mtx);
    satellites.erase(satId);
}

Connection* GCConnectionHandler::getConnection(int satId) {
    // get a connection at a specific id
    std::lock_guard<std::mutex>lock(this->mtx);
    auto it = satellites.find(satId);
    if (it == satellites.end()) return nullptr;
    return &it->second;
}

void GCConnectionHandler::sendMessageToSat(int satId, const Message& message) const {
    // send a message to a specific satellite
    std::lock_guard<std::mutex>lock(this->mtx);
    auto satellite = satellites.find(satId);
    if (satellite == satellites.end()) {
        // push message to logger queue
        std::cout << "Satellite Id: " << satId << " Not found" << std::endl;
        return;
    }
    satellite->second.sendMessage(message);
}

void GCConnectionHandler::broadcastMessage(const Message& message) {
    // loop through all Satellites and send them a message
    std::lock_guard<std::mutex>lock(this->mtx);
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
    std::lock_guard<std::mutex>lock(this->mtx);
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