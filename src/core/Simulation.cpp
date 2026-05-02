/*
File: Simulation
Date Created: March 25th, 2026
Last Updated: April 21st, 2026
Purpose: This file runs the simulation with the time step, and updates this satellite's position and velocity accordingly
*/

#include "Simulation.h"

Simulation::Simulation(double timeStep, Satellite& satellite) 
    : timeStep(timeStep), satellite(satellite) {
}

void Simulation::run() {
    while (true) {
        // update the satellite and print its position and velocity
        satellite.update(timeStep);
        satellite.print();
        usleep(1000000); // 1 second pause
    }
}