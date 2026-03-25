// .h file for the Satellite class, it contains the declaration of the class and its functions

#ifndef SIMULATION_H
#define SIMULATION_H    
#include "Satellite.h"
#include <vector>

class Simulation {
    private:
        std::vector<Satellite> satellites;
        double timeStep;

    public:
        // Constructor for the Simulation class, takes a time step as arguments
        Simulation(double timeStep);

        // adds a satellite to the simulation
        void addSatellite(Satellite s);

        // Run function for the Simulation class, it runs an infinite loop that updates every satellite's position and velocity 
        //every time step
        void run();
};

#endif