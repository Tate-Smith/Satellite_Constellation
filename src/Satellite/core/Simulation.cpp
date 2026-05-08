/*
File: Simulation
Date Created: March 25th, 2026
Last Updated: May 7th, 2026
Purpose: This file runs the simulation with the time step, and updates this satellite's position and velocity accordingly
*/

#include "Simulation.h"

Simulation::Simulation(double timeStep, Satellite& satellite, std::atomic<bool> *running) : timeStep(timeStep), satellite(satellite), running(running) {
    assert(timeStep > 0);
    assert(running != nullptr);
}

void Simulation::run() {
    /*
    This method is run by a thread and increments a satellites information every second 
    */
    assert(timeStep > 0);
    assert(running != nullptr);
    while (running->load()) {
        // update the satellite and print its position and velocity
        satellite.update(timeStep);
        satellite.print();
        sleep(1); // 1 second pause
    }
}