// .h file for Satellite class, defines the class and its functions

#ifndef SATELLITE_H
#define SATELLITE_H
#include <iostream>

class Satellite {
    private:
        int id;
        double x, y, z;
        double vx, vy, vz;

    public:
        // contructor
        Satellite(int id, double x, double y, double z, double vx, double vy, double vz);

        // getters
        int getId() const;

        double getX() const;

        double getY() const;

        double getZ() const;

        // update function
        void update(double dt);

        // get the distance of the satellite from another satellite
        double distance(const Satellite& other) const;

        // print function, gets the id and position and velocity
        void print() const;
};

#endif