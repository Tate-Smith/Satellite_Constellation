/*
File: Satellite
Date Created: March 25th, 2026
Last Updated: March 30th, 2026
Author: Tate Smith
Purpose: This file represents a Satellite node in the constellation, it can send and receive 
information from other satellites and ground control
*/

#include "Satellite.h"
#include <cmath>

Satellite::Satellite(uint32_t id, double x, double y, double z, double vx, double vy, double vz) {
    this->id = id;
    this->x = x;
    this->y = y;
    this->z = z;
    this->vx = vx;
    this->vy = vy;
    this->vz = vz;
}

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

void Satellite::update(double dt) {
    x += vx * dt;
    y += vy * dt;
    z += vz * dt;
}

double Satellite::distance(const Satellite& other) const {
    return sqrt(pow(other.getX() - this->x, 2) + pow(other.getY() - this->y, 2) + pow(other.getZ() - this->z, 2));
}

void Satellite::print() const {
    std::cout << "Satellite " << id << ": Position (" << x << ", " << y << ", " << z << ") Velocity (" << vx << ", " << vy << ", "
    << vz << ")" << std::endl;
}

Message Satellite::createStatusMessage() const {
    Message message;
    message.type = MessageType::STATUS_UPDATE;
    message.senderId = id;
    message.x = x;
    message.y = y;
    message.z = z;
    return message;
}

Message Satellite::createHeartbeatMessage() const {
    Message message;
    message.type = MessageType::HEARTBEAT;
    message.senderId = id;
    message.x = x;
    message.y = y;
    message.z = z;
    return message;
}