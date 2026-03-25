// .h file for the Satellite class, it contains the declaration of the class and its functions

#ifndef SIMULATION_H
#define SIMULATION_H    
#include "Satellite.h"

class Simulation {
    private:
        Satellite s;
        double timeStep;

    public:
        // Constructor for the Simulation class, takes a Satellite and a time step as arguments
        Simulation(const Satellite& s, double timeStep);

        // Run function for the Simulation class, it runs an infinite loop that updates the satellite's position and velocity 
        //every time step
        void run();
};

#endif