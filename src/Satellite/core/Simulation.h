// .h file for the Satellite class, it contains the declaration of the class and its functions

#ifndef SIMULATION_H
#define SIMULATION_H  
  
#include "Satellite.h"
#include <unistd.h>

class Simulation {
    private:
        double timeStep;
        Satellite& satellite;
        std::atomic<bool> *running;

    public:
        // Constructor for the Simulation class, takes a time step as arguments
        Simulation(double timeStep, Satellite& satellite, std::atomic<bool> *running); 

        // Run function for the Simulation class, it runs an infinite loop that updates every satellite's position and velocity every time step
        void run();
};

#endif