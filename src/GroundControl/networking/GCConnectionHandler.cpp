/*
File: GCConnectionHandler
Date Created: April 9th, 2026
Last Updated: May 7th, 2026
Author: Tate Smith
Purpose: This file handles all the satellite connections with ground control and methods to communicate with them
*/

#include "GCConnectionHandler.h"

GCConnectionHandler::GCConnectionHandler(MessageQueue<std::string> *loggerQueue, MessageQueue<SatOutput> *outputQueue, MessageQueue<CommandInput> *inputQueue, 
std::atomic<bool> *running, int gcPort) : gcPort(gcPort), loggerQueue(loggerQueue), outputQueue(outputQueue), inputQueue(inputQueue), running(running) {
    assert(loggerQueue != nullptr);
    assert(outputQueue != nullptr);
    assert(inputQueue != nullptr);
    assert(running != nullptr);
}

void GCConnectionHandler::addConnection(int port, const std::string &ip, int satId) {
    /*
    This method takes in a satellites port, ip, and its id, along with the ground controls port. It then creates a
    Connection object with the given information and adds it to the connections map
    */
    assert(port >= 0);
    assert(satId > 0);
    assert(!ip.empty());
    assert(gcPort >= 0);
    assert(port != gcPort);
    assert(loggerQueue != nullptr);
    // create a new connection and add it to the map, and connect to it if it works
    std::lock_guard<std::mutex>lock(this->mtx);
    auto [it, inserted] = satellites.emplace(satId, Connection(satId, port, ip, gcPort, loggerQueue));
    if (inserted) {
        it->second.connect();
    }
}

void GCConnectionHandler::update() {
    /*
    This method is ran via the update thread, it constantly checks the status' of the satellites in the 
    connection map, and if they have disonnected, attempt to reconnect until deemed dead
    */
    assert(loggerQueue != nullptr);
    assert(outputQueue != nullptr);
    while (running->load()) {
        {
            // loop through every connection in the map, and if they are disconnected and outgoing try to reconnect
            std::lock_guard<std::mutex>lock(this->mtx);
            for (auto& i : satellites) {
                // check if satellites are disconnected
                if (i.second.getState() == GCConnectionState::CONNECTED && i.second.isTimedOut()) {
                    // push message to logger queue
                    loggerQueue->pushBack("[NETWORK] Satellite Id: " + std::to_string(i.first) + " has timed out");
                    i.second.disconnect();
                }

                // try to reconnect if disconnected
                else if (i.second.getState() == GCConnectionState::DISCONNECTED && !i.second.isDead()) {
                    if (!i.second.reconnect()) {
                        outputQueue->pushBack(parser.dataDecoder(i.first, emptyChunk, false)); // tell the terminal the sat is dead
                    }
                }
            }
        }
        // wait 1 second
        sleep(1);
    }
}

void GCConnectionHandler::removeConnection(int satId) {
    /*
    This method takes in a satellite id and removes it from the connections map if already in it
    */
    assert(satId > 0);
    std::lock_guard<std::mutex>lock(this->mtx);
    assert(satellites.contains(satId));
    satellites.erase(satId);
}

bool GCConnectionHandler::hasConnection(int satId) {
    /*
    This method takes in a satellite id and checks if the satelllite is in the connection map, meaning its connected
    */
    assert(satId > 0);
    std::lock_guard<std::mutex> lock(this->mtx);
    return satellites.find(satId) != satellites.end();
}

void GCConnectionHandler::heartbeatSat(int satId) {
    /*
    This method takes in a satellite id and updates its heartbeat and connection status to connected
    */
    assert(satId > 0);
    std::lock_guard<std::mutex>lock(this->mtx);
    auto it = satellites.find(satId);
    if (it != satellites.end()) {
        it->second.heartbeat();
        it->second.markConnected();
    }
}

void GCConnectionHandler::sendMessageToSat(int satId, const Message& message) {
    /*
    This method teakes in a satellite id and a message reference, it then finds that satellite in the
    connection map and sends the message to it
    */
    assert(message.header.size > 0);
    assert(message.senderPort >= 0);
    assert(loggerQueue != nullptr);
    // send a message to a specific satellite
    std::lock_guard<std::mutex>lock(this->mtx);
    auto satellite = satellites.find(satId);
    if (satellite == satellites.end()) {
        // push message to logger queue
        loggerQueue->pushBack("[ERROR] Satellite Id: " + std::to_string(satId) + " Not found");
        return;
    }
    satellite->second.sendMessage(message);
}

void GCConnectionHandler::sendCommand() {
    /*
    This method takes in the ground controls port, it checks the input queue for any new satellite
    command messages, and waits until there are messages available, it then creates a command stuct
    and sends that message to the correct satellite
    */
    assert(gcPort >= 0);
    assert(inputQueue != nullptr);
    while (running->load()) {
        // block until there are commands
        CommandInput com = inputQueue->pop();
        if (!running->load()) break;
        // create a new command
        Command c;
        c.header.type = MessageType::COMMAND;
        c.header.size = sizeof(c);
        c.senderId = 0;
        c.senderPort = gcPort;
        c.x = com.new_x;
        c.y = com.new_y;
        c.z = com.new_z;
        c.vx = com.new_vx;
        c.vy = com.new_vy;
        c.vz = com.new_vz;
        // send the command to the correct satellite
        this->sendMessageToSat(com.id, c);
    }
}