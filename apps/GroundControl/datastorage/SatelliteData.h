// the .h file representing the data of a satellite for ground control use

#ifndef SATELLITE_DATA_H
#define SATELLITE_DATA_H

#include <string>
#include "../../../src/logging/Logger.h"
#include <thread>

class SatelliteData {
    private:
        uint32_t id;
        double x;
        double y;
        double z;
        double vx;
        double vy;
        double vz;
        int packetsRecieved;
        int packetsSent;
        bool alive;
        std::atomic<bool> running{true};
        MessageQueue<std::string> loggerQueue;
        Logger logger;
        std::thread loggerThread;

    public:
        SatelliteData(int id);
        void updateVals(double x, double y, double z, double vx, double vy, double vz);
        void updatePackets(int received, int sent);
        void markAlive(bool b);
        bool isAlive();
        int getSatId();
        std::string toString() const;
        void start(); // has the logger log info
};

#endif