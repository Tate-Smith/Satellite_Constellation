/*
File: Simulation
Date Created: March 25th, 2026
Last Updated: March 30th, 2026
Purpose: This file runs the simulation with the time step, and updates all satellites accordingly
*/

#include "Simulation.h"
#include <unistd.h>

Simulation::Simulation(double timeStep, const Satellite& satellite) 
    : timeStep(timeStep), satellite(satellite) {}

void Simulation::run(NetworkManager& networkManager) {
    int i = 0;
    while (true) {
        // update the satellite and print its position and velocity
        satellite.update(timeStep);
        satellite.print();
        // send a status message to the target peer every 5 steps
        if (i % 5 == 0) {
            networkManager.sendMessage(satellite.createStatusMessage());
            // receive a message from the target peer
            networkManager.receiveMessage(); 
        }
        sleep(1);

        // increment i
        i++;
    }
}