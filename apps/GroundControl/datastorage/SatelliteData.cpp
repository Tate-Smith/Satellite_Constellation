/*
File: SatelliteData
Date Created: April 9th, 2026
Last Updated: April 29th, 2026
Author: Tate Smith
Purpose: This file rholds the information for each satellite object the ground control is connected to
*/

#include "SatelliteData.h"

SatelliteData::SatelliteData(int id) : id(id), x(0), y(0), z(0), vx(0), vy(0), vz(0), packetsRecieved(0), packetsSent(0), alive(true) {}

void SatelliteData::updatePos(int x, int y, int z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

void SatelliteData::updateVel(int vx, int vy, int vz) {
    this->vx = vx;
    this->vy = vy;
    this->vz = vz;
}

void SatelliteData::updateRecieved() {
    this->packetsRecieved++;
}

void SatelliteData::updateSent() {
    this->packetsSent++;
}

void SatelliteData::markAlive(bool b) {
    this->alive = b;
}

bool SatelliteData::isAlive() {
    return this->alive;
}

int SatelliteData::getSatId() {
    return this->id;
}

std::string SatelliteData::toString() const {
    std::string satAlive = this->alive ? "alive" : "dead";
    return "Satellited Id: " + std::to_string(this->id) +"; Position: [" + std::to_string(this->x) + ", " + std::to_string(this->y) + ", " 
    + std::to_string(this->z) + " | Velocity: " + std::to_string(this->vx) + ", " + std::to_string(this->vy) + ", " + std::to_string(this->vz) 
    + " | Packets sent/Recieved: " + std::to_string(this->packetsSent) + "/" + std::to_string(this->packetsRecieved) + " | Status: " 
    + satAlive + " ]\n";
}