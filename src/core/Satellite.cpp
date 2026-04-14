/*
File: Satellite
Date Created: March 25th, 2026
Last Updated: April 9th, 2026
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

Message Satellite::createStatusMessage() const {
    Status_Message message{};
    message.type = MessageType::STATUS_UPDATE;
    message.senderId = id;
    message.x = x;
    message.y = y;
    message.z = z;
    message.vx = vx;
    message.vy = vy;
    message.vz = vz;
    return message;
}

Message Satellite::createHeartbeatMessage() const {
    Heartbeat message{};
    message.type = MessageType::HEARTBEAT;
    message.senderId = id;
    // get current time stamp
    message.timestamp = time(nullptr);
    message.alive = true;
    return message;
}

ConnectionHandler* Satellite::getConnectionHandler() {
    return &this->handler;
}