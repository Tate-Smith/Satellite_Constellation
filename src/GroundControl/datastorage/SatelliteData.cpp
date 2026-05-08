/*
File: SatelliteData
Date Created: April 9th, 2026
Last Updated: May 8th, 2026
Author: Tate Smith
Purpose: This file holds the information for each satellite object the ground control is connected to
*/

#include "SatelliteData.h"

SatelliteData::SatelliteData(int id, std::atomic<bool> *running) : id(id), x(0.0), y(0.0), z(0.0), vx(0.0), vy(0.0), vz(0.0), packetsReceived(0), 
packetsSent(0), alive(true), running(running), logger("output/GC_Satellite_" + std::to_string(id) + "_logger.txt", &loggerQueue, running) {}

void SatelliteData::updateVals(double newx, double newy, double newz, double newvx, double newvy, double newvz) {
    /*
    This method takes in new coordinate and velocity vector values and updates the instance variables to
    hold this information
    */
    this->x = newx;
    this->y = newy;
    this->z = newz;
    this->vx = newvx;
    this->vy = newvy;
    this->vz = newvz;
}

void SatelliteData::updatePackets(int received, int sent) {
    /*
    This method takes in two ints, received and sent, and updates those instnace variables accordingly
    */
    assert(received >= 0);
    assert(sent >= 0);
    this->packetsReceived += received;
    this->packetsSent += sent;
}

void SatelliteData::markAlive(bool b) {
    /*
    This method takes in a boolean and updates the alive status of the satellite based off of thsi
    */
    assert(id > 0);
    this->alive = b;
}

bool SatelliteData::isAlive() {
    /*
    This method gets the status of the alive instance variable
    */
    assert(id > 0);
    return this->alive;
}

int SatelliteData::getSatId() {
    /*
    This method gets the value of the satellites id
    */
    assert(id > 0);
    return this->id;
}

std::string SatelliteData::toString() const {
    /*
    This method returns a string formatted version of the satellites data, in a user readable format
    */
    assert(id > 0);
    std::string satAlive = this->alive ? "alive" : "dead";
    return "Satellite: " + std::to_string(this->id) +"; Position: [" + std::to_string(this->x) + ", " + std::to_string(this->y) + ", " 
    + std::to_string(this->z) + " | Velocity: " + std::to_string(this->vx) + ", " + std::to_string(this->vy) + ", " + std::to_string(this->vz) 
    + " | Packets sent/received: " + std::to_string(this->packetsSent) + "/" + std::to_string(this->packetsReceived) + " | Status: " 
    + satAlive + "]\n";
}

void SatelliteData::start() {
    /*
    This method starts the logger thread for the satellites stored data
    */
    assert(running != nullptr);
    this->loggerThread = std::thread(&Logger::log, &logger);
}

void SatelliteData::logData(const std::vector<char> &lines) {
    /*
    This method takes in a char vector that holds all the satellites logs, and logs all of that data
    in the ground control satellite logs for stored use
    */
    std::string line;
    for (char c : lines) {
        if (c == '\n') {
            this->loggerQueue.pushBack(line);
            line.clear();
        }
        else line += c;
    }
}

SatelliteData::~SatelliteData() {
    /*
    This method is the destructor and turns off the running boolean, shutsdown the logger queue and joins the
    logger thread
    */
    assert(id > 0);
    loggerQueue.shutdown();
    if (loggerThread.joinable()) loggerThread.join();
}