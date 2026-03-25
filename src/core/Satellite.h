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

        // update function
        void update(double dt);

        // print function, gets the id and position and velocity
        void print();
};

#endif