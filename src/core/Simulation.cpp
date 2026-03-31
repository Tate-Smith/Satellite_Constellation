/*
File: Simulation
Date Created: March 25th, 2026
Last Updated: March 31st, 2026
Purpose: This file runs the simulation with the time step, and updates all satellites accordingly
*/

#include "Simulation.h"
#include <unistd.h>

Simulation::Simulation(double timeStep, const Satellite& satellite) 
    : timeStep(timeStep), satellite(satellite) {}

void Simulation::run(NetworkManager& networkManager) {
    int i = 0;
    while (true) {
        // accept all incoming connections/messages
        networkManager.acceptConnections(*satellite.getConnectionHandler());
        // update the satellite and print its position and velocity
        satellite.update(timeStep);
        satellite.print();
        // send a heartbeat message to the target peer every 5 steps
        if (i % 5 == 0) {
            satellite.getConnectionHandler()->broadcastMessage(satellite.createHeartbeatMessage());
        }
        sleep(1);
        // increment i
        i++;
    }
}