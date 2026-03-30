// .h file for Satellite class, defines the class and its functions

#ifndef SATELLITE_H
#define SATELLITE_H
#include <iostream>
#include "../protocol/Message.h"

class Satellite {
    private:
        uint32_t id;
        double x, y, z;
        double vx, vy, vz;

    public:
        // constructor
        Satellite(uint32_t id, double x, double y, double z, double vx, double vy, double vz);

        // getters
        uint32_t getId() const;

        double getX() const;

        double getY() const;

        double getZ() const;

        // update function
        void update(double dt);

        // get the distance of the satellite from another satellite
        double distance(const Satellite& other) const;

        // print function, gets the id and position and velocity
        void print() const;

        // create a status message for this satellite
        Message createStatusMessage() const;

        // create a heartbeat message for this satellite
        Message createHeartbeatMessage() const;
};

#endif