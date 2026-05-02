/*
File: GCConnectionHandler
Date Created: April 9th, 2026
Last Updated: Mat 2nd, 2026
Author: Tate Smith
Purpose: This file handles all conenctions with the ground control
*/

#include "GCConnectionHandler.h"

GCConnectionHandler::GCConnectionHandler(MessageQueue<std::string> *logger_queue, MessageQueue<SatOutput> *output_queue, MessageQueue<CommandInput> *input_queue) : 
logger_queue(logger_queue), output_queue(output_queue), input_queue(input_queue) {}

void GCConnectionHandler::addConnection(int port, const std::string &ip, int satId, int gcPort) {
    // create a new connection and add it to the map, and connect to it if it works
    std::lock_guard<std::mutex>lock(this->mtx);
    auto [it, inserted] = satellites.emplace(satId, Connection(satId, port, ip, gcPort, logger_queue));
    if (inserted) {
        it->second.connect();
    }
}

void GCConnectionHandler::update() {
    while (true) {
        {
            // loop through every connection in the map, and if they are disconnected and outgoing try to reconnect
            std::lock_guard<std::mutex>lock(this->mtx);
            for (auto& i : satellites) {
                // check if satellites are disconnected
                if (i.second.getState() == GCConnectionState::CONNECTED && i.second.isTimedOut()) {
                    // push message to logger queue
                    logger_queue->pushBack("Satellite Id: " + std::to_string(i.first) + " has timed out");
                    i.second.disconnect();
                }

                // try to reconnect if disconnected
                else if (!i.second.isDead() && i.second.getState() == GCConnectionState::DISCONNECTED) {
                    if (!i.second.reconnect()) {
                        std::vector<char> empty;
                        output_queue->pushBack(parser.dataDecoder(i.first, empty, false)); // tell the terminal the sat is dead
                    }
                }
            }
        }
        // wait 1 second
        sleep(1);
    }
}

void GCConnectionHandler::removeConnection(int satId) {
    // remove a connection based off of its id
    std::lock_guard<std::mutex>lock(this->mtx);
    satellites.erase(satId);
}

bool GCConnectionHandler::hasConnection(int satId) {
    std::lock_guard<std::mutex> lock(this->mtx);
    return satellites.find(satId) != satellites.end();
}

void GCConnectionHandler::heartbeatSat(int satId) {
    std::lock_guard<std::mutex>lock(this->mtx);
    auto it = satellites.find(satId);
    if (it != satellites.end()) {
        it->second.heartbeat();
        it->second.markConnected();
    }
}

void GCConnectionHandler::sendMessageToSat(int satId, const Message& message) const {
    // send a message to a specific satellite
    std::lock_guard<std::mutex>lock(this->mtx);
    auto satellite = satellites.find(satId);
    if (satellite == satellites.end()) {
        // push message to logger queue
        logger_queue->pushBack("Satellite Id: " + std::to_string(satId) + " Not found");
        return;
    }
    satellite->second.sendMessage(message);
}

void GCConnectionHandler::sendCommands() {
    while (true) {
        // block until there are commands
        CommandInput com = this->input_queue->pop();
        // create a new command
        Command c;
        c.header.type = MessageType::COMMAND;
        c.header.size = sizeof(c);
        c.senderId = 0;
        c.senderPort = 8000;
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