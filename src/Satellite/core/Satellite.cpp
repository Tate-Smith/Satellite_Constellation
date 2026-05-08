/*
File: Satellite
Date Created: March 25th, 2026
Last Updated: May 7th, 2026
Author: Tate Smith
Purpose: This file represents a Satellite node in the constellation, it can send and receive 
information from other satellites and ground control. It owns a connection handler to manage the 
other satellites its connected too along with the ground control
*/

#include "Satellite.h"

Satellite::Satellite(uint32_t id, double x, double y, double z, double vx, double vy, double vz, int port, MessageQueue<std::string> *loggerQueue) : 
handler(loggerQueue, port), loggerQueue(loggerQueue), port(port) {
    /*
    This is the contructor for the satellite and it instantiates all the instance variables
    */
    assert(id > 0);
    assert(port >= 0);
    assert(loggerQueue != nullptr);
    this->id = id;
    this->x = x;
    this->y = y;
    this->z = z;
    this->vx = vx;
    this->vy = vy;
    this->vz = vz;
    this->filename = "output/Satellite_" + std::to_string(id) + "_logger.txt";
}

// getters
uint32_t Satellite::getId() const{
    assert(id > 0);
    return id;
}

double Satellite::getX() const {
    assert(id > 0);
    return x;
}

double Satellite::getY() const {
    assert(id > 0);
    return y;
}

double Satellite::getZ() const {
    assert(id > 0);
    return z;
}

int Satellite::getPort() {
    return port;
}

ConnectionHandler* Satellite::getConnectionHandler() {
    assert(id > 0);
    assert(port >= 0);
    return &this->handler;
}

bool Satellite::getWaitingForAck() {
    assert(id > 0);
    return this->waitingForAck;
}

void Satellite::connectToPeer(const std::string& ip, int peerPort, uint32_t peerId) {
    /*
    This method takes in a const unmodifiable string address, a port number, and a peerid, it
    then uses the connectionHandler object to add a new outgoing peer to the peers list
    */
    assert(!ip.empty());
    assert(peerPort >= 0);
    assert(peerId >= 0);
    handler.addConnection(peerPort, ip, peerId, this->id);
}

void Satellite::update(double dt) {
    /*
    This method takes in a double step value and multiplies the velocity by it and incements the 
    x,y,z coordinates accordingly
    */
    assert(dt > 0);
    x += vx * dt;
    y += vy * dt;
    z += vz * dt;
    handler.update();
}

void Satellite::print() {
    /*
    This method prints a human readable version of the satellites data
    */
    assert(loggerQueue != nullptr);
    std::string str = "SAT " + std::to_string(id) + " POS " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) 
    + " VEL " + std::to_string(vx) + " " + std::to_string(vy) + " " + std::to_string(vz);
    // push the string onto the loggers message queue
    loggerQueue->pushBack(std::move(str));
}

Heartbeat Satellite::createHeartbeatMessage() const {
    /*
    This method creates a heartbeat message with all of the satellites data and returns it
    */
    assert(id > 0);
    assert(port >= 0);
    Heartbeat m;
    m.header.type = MessageType::HEARTBEAT;
    m.header.size = sizeof(m);
    m.senderId = id;
    m.senderPort = port;
    m.alive = true;
    return m;
}

std::vector<File_Msg> Satellite::createDataDump() {
    /*
    This function creates a File_Msg struct with all the information from the satellites logger file
    */
    // open the file
    assert(!filename.empty());
    std::ifstream file(filename , std::ios::binary);

    // make sure it opened succesfully
    if (!file) throw std::runtime_error("File failed to open");

    // create the vector to return
    std::vector<File_Msg> msgs;
    const int MAX_CHUNKS = 100;
    int i = 0;
    while (i < MAX_CHUNKS) {
        // create message and return it
        File_Msg m;
        m.header.type = MessageType::FILE_MSG;
        m.senderId = this->id;
        m.senderPort = port;
        file.read(m.data, sizeof(m.data));
        m.len = file.gcount();
        m.last = file.eof();
        m.header.size = sizeof(m);
        bool isLast = m.last;
        msgs.push_back(std::move(m));
        if (isLast) break;
        i++;
    }
    // clear the file
    file.close();
    std::ofstream clear(filename, std::ios::trunc);
    // check if succesfully cleared
    if (!clear) {
        throw std::runtime_error("File failed to clear");
    }
    return msgs;
}

void Satellite::setWaitingForAck(bool b) {
    /*
    This method takes in a boolean and sets the value of waiting for ack to that boolean, it represents whether 
    the satellite is waiting for an acknowledgement from a peer or ground control after sending a message
    */
    this->waitingForAck = b;
}

void Satellite::handleCommand(const Command &cmd) {
    /*
    This method takes in a command and it updates the satellites values based on that command
    */
    assert(cmd.header.size > 0);
    assert(cmd.senderId == 0);
    assert(cmd.senderPort >= 0);
    assert(cmd.senderPort != this->port);
    this->x = cmd.x;
    this->y = cmd.y;
    this->z = cmd.z;
    this->vx = cmd.vx;
    this->vy = cmd.vy;
    this->vz = cmd.vz;
}
