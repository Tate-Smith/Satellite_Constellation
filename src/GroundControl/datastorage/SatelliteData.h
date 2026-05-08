// the .h file representing the data of a satellite for ground control use

#ifndef SATELLITE_DATA_H
#define SATELLITE_DATA_H

#include <string>
#include <cassert>
#include <thread>
#include "../../logging/Logger.h"

class SatelliteData {
    private:
        uint32_t id;
        double x, y, z, vx, vy, vz;
        int packetsReceived, packetsSent;
        bool alive;
        std::atomic<bool> *running; // to track whether the program is still running
        MessageQueue<std::string> loggerQueue;
        Logger logger;
        std::thread loggerThread;

    public:
        SatelliteData(int id, std::atomic<bool> *running);
        void updateVals(double newx, double newy, double newz, double newvx, double newvy, double newvz);
        void updatePackets(int received, int sent);
        void markAlive(bool b);
        bool isAlive();
        int getSatId();
        std::string toString() const; // creates a human readable format for the data
        void start(); // has the logger log info
        void logData(const std::vector<char> &lines); // logs all the data in the vector
        ~SatelliteData();
};

#endif