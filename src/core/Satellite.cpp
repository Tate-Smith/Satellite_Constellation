/*
File: Satellite
Date Created: March 25th, 2026
Last Updated: May 1st, 2026
Author: Tate Smith
Purpose: This file represents a Satellite node in the constellation, it can send and receive 
information from other satellites and ground control
*/

#include "Satellite.h"

Satellite::Satellite(uint32_t id, double x, double y, double z, double vx, double vy, double vz, int port, MessageQueue<std::string> *queue) : 
handler(queue, port), queue(queue), port(port) {
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
    handler.addConnection(port, ip, peerId, this->id);
}

void Satellite::update(double dt) {
    x += vx * dt;
    y += vy * dt;
    z += vz * dt;
    handler.update();
}

void Satellite::print() const {
    std::string str = "SAT " + std::to_string(id) + " POS " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) 
    + " VEL " + std::to_string(vx) + " " + std::to_string(vy) + " " + std::to_string(vz);
    // push the string onto the loggers message queue
    queue->pushBack(str);
}

Heartbeat Satellite::createHeartbeatMessage() const {
    Heartbeat m;
    m.header.type = MessageType::HEARTBEAT;
    m.header.size = sizeof(m);
    m.senderId = id;
    m.senderPort = port;
    m.alive = true;
    return m;
}

std::vector<File_Msg> Satellite::createDataDump() {
    // this is the function that the satellite uses to send all of its file data down to the GC
    // open the file
    std::string filename = "Satellite_" + std::to_string(this->id) + "_logger.txt";
    std::ifstream file(filename , std::ios::binary);

    // make sure it opened succesfully
    if (!file) throw std::runtime_error("File failed to open");

    // create the vector to return
    std::vector<File_Msg> msgs;

    while (true) {
        // create message and return it
        File_Msg m;
        m.header.type = MessageType::FILE_MSG;
        m.senderId = this->id;
        file.read(m.data, sizeof(m.data));
        m.len = file.gcount();
        m.last = file.eof();
        m.header.size = sizeof(m);
        msgs.push_back(m);
        if (m.last) break;
    }
    // clear the file
    file.close();
    std::ofstream clear(filename, std::ios::trunc);
    return msgs;
}

ConnectionHandler* Satellite::getConnectionHandler() {
    return &this->handler;
}

void Satellite::setWaitingForAck(bool b) {
    this->waitingForAck = b;
}

bool Satellite::getWaitingForAck() {
    return this->waitingForAck;
}

void Satellite::handleCommand(Command cmd) {
    this->x = cmd.x;
    this->y = cmd.y;
    this->z = cmd.z;
    this->vx = cmd.vx;
    this->vy = cmd.vy;
    this->vz = cmd.vz;
}
