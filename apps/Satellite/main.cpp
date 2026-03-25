/*
File: Satellite
Date Created: March 25th, 2026
Last Updated: March 25th, 2026
Purpose: This file runs the simulation with the time step, and updates all satellites accordingly
*/

#include "../../src/core/Satellite.h"
#include "../../src/core/Simulation.h"

int main() {
    // create a satellite with id 1, position (0,0,0) and velocity (1,1,1)
    Satellite s(1, 0, 0, 0, 1, 1, 1);
    // create a simulation with the satellite and a time step of 1 second
    Simulation sim(s, 1);
    // run the simulation
    sim.run();

    return 0;
}