// the .h file representing the data of a satellite for ground control use

#ifndef SATELLITE_DATA_H
#define SATELLITE_DATA_H

#include <string>

class SatelliteData {
    private:
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
        
};

#endif