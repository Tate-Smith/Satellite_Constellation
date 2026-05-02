/*
File: SatelliteData
Date Created: April 9th, 2026
Last Updated: May 2nd, 2026
Author: Tate Smith
Purpose: This file rholds the information for each satellite object the ground control is connected to
*/

#include "SatelliteData.h"

SatelliteData::SatelliteData(int id) : id(id), x(0.0), y(0.0), z(0.0), vx(0.0), vy(0.0), vz(0.0), 
packetsRecieved(0), packetsSent(0), alive(true), logger("GC_Satellite_" + std::to_string(id) + "_logger.txt", &loggerQueue, &running) {}

void SatelliteData::updateVals(double x, double y, double z, double vx, double vy, double vz) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->vx = vx;
    this->vy = vy;
    this->vz = vz;
    loggerQueue.pushBack("SAT " + std::to_string(id) + " POS " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) 
    + " VEL " + std::to_string(vx) + " " + std::to_string(vy) + " " + std::to_string(vz));
}

void SatelliteData::updatePackets(int received, int sent) {
    this->packetsRecieved += received;
    this->packetsSent += sent;
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
    return "Satellite: " + std::to_string(this->id) +"; Position: [" + std::to_string(this->x) + ", " + std::to_string(this->y) + ", " 
    + std::to_string(this->z) + " | Velocity: " + std::to_string(this->vx) + ", " + std::to_string(this->vy) + ", " + std::to_string(this->vz) 
    + " | Packets sent/recieved: " + std::to_string(this->packetsSent) + "/" + std::to_string(this->packetsRecieved) + " | Status: " 
    + satAlive + "]\n";
}

void SatelliteData::start() {
    this->loggerThread = std::thread(&Logger::log, &logger);
    loggerThread.detach();
}