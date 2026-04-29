// the .h file representing the data of a satellite for ground control use

#ifndef SATELLITE_DATA_H
#define SATELLITE_DATA_H

#include <string>

class SatelliteData {
    private:
    int id;
        double x;
        double y;
        double z;
        double vx;
        double vy;
        double vz;
        int packetsRecieved;
        int packetsSent;
        bool alive;

    public:
        SatelliteData(int id);
        void updatePos(double x, double y, double z);
        void updateVel(double vx, double vy, double vz);
        void updateRecieved();
        void updateSent();
        void markAlive(bool b);
        bool isAlive();
        int getSatId();
        std::string toString() const;
};

#endif