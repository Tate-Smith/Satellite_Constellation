/*
File: Satellite
Date Created: March 25th, 2026
Last Updated: March 25th, 2026
Purpose: This file runs the simulation with the time step, and updates all satellites accordingly
*/

#include "../../src/core/Satellite.h"
#include "../../src/core/Simulation.h"

int main() {
    // create a bunch of satellites with different positions and velocities
    Satellite s1(1, 0, 0, 0, 1, 1, 1);
    Satellite s2(2, 1, 1, 1, 2, 2, 2);
    Satellite s3(3, 2, 2, 2, 3, 3, 3);
    Satellite s4(4, 3, 3, 3, 4, 4, 4);
    Satellite s5(5, 4, 4, 4, 5, 5, 5);
    Satellite s6(6, 5, 5, 5, 6, 6, 6);
    Satellite s7(7, 6, 6, 6, 7, 7, 7);
    Satellite s8(8, 7, 7, 7, 8, 8, 8);
    Satellite s9(9, 8, 8, 8, 9, 9, 9);
    Satellite s10(10, 9, 9, 9, 10, 10, 10);

    // create a simulation with a time step of 1 second
    Simulation sim(1);
    // add the satellites to the simulation
    sim.addSatellite(s1);
    sim.addSatellite(s2);
    sim.addSatellite(s3);
    sim.addSatellite(s4);
    sim.addSatellite(s5);
    sim.addSatellite(s6);
    sim.addSatellite(s7);
    sim.addSatellite(s8);
    sim.addSatellite(s9);
    sim.addSatellite(s10);

    // run the simulation
    sim.run();

    return 0;
}