/*
File: Satellite
Date Created: March 25th, 2026
Last Updated: April 15th, 2026
Author: Tate Smith
Purpose: This file represents a Satellite node in the constellation, it can send and receive 
information from other satellites and ground control
*/

#include "Satellite.h"
#include <cmath>

Satellite::Satellite(uint32_t id, double x, double y, double z, double vx, double vy, double vz, MessageQueue *queue) : handler(queue), queue(queue) {
    this->id = id;
    this->x = x;
    this->y = y;
    this->z = z;
    this->vx = vx;
    this->vy = vy;
    this->vz = vz;
}

// getters
uint32_t Satellite::getId() const{
    return id;
}

double Satellite::getX() const {
    return x;
}

double Satellite::getY() const {
    return y;
}

double Satellite::getZ() const {
    return z;
}

void Satellite::connectToPeer(const std::string& ip, int port, uint32_t peerId) {
    /*
    This function takes in a const unmodifiable string address, a port number, and a peerid, it
    then uses the connectionHandler object to add a new outgoing peer to the peers list
    */
    handler.addOutgoingConnection(port, ip, peerId, this->id);
}

void Satellite::update(double dt) {
    x += vx * dt;
    y += vy * dt;
    z += vz * dt;
    handler.update();
}

void Satellite::print() const {
    std::string str = "Satellite " + std::to_string(id) + ": Position (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) 
    + ") Velocity (" + std::to_string(vx) + ", " + std::to_string(vy) + ", " + std::to_string(vz) + ")";
    // push the string onto the loggers message queue
    queue->pushBack(str);
}

Message Satellite::createHeartbeatMessage() const {
    Heartbeat m;
    m.header.type = MessageType::HEARTBEAT;
    m.header.size = sizeof(m);
    m.senderId = id;
    // get current time stamp
    m.timestamp = time(nullptr);
    m.alive = true;
    return m;
}

Message Satellite::createdataDump() const {
    // this is the function that the satellite uses to send all of its file data down to the GC
    // open the file
    std::string filename = "Satelite_" + std::to_string(this->id) + "_logger.txt";
    std::ifstream file(filename , std::ios::binary);

    // makesure it opened succesfully
    if (!file) {
        std::cerr << "File failed to open" << std::endl;
        exit(1);
    }

    // create message and return it
    File_Msg m;
    m.header.type = MessageType::FILE_MSG;
    m.senderId = this->id;
    file.read(m.data, sizeof(m.data));
    m.len = file.gcount();
    m.header.size = sizeof(m);

    // clear the file
    file.clear();

    return m;
}

ConnectionHandler* Satellite::getConnectionHandler() {
    return &this->handler;
}