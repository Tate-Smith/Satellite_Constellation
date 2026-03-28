/*
File: Simulation
Date Created: March 25th, 2026
Last Updated: March 28th, 2026
Purpose: This file runs the simulation with the time step, and updates all satellites accordingly
*/

#include "Simulation.h"
#include <unistd.h>

Simulation::Simulation(double timeStep) {
    this->timeStep = timeStep;
}

void Simulation::addSatellite(Satellite s) {
    satellites.push_back(s);
}

void Simulation::run() {
    while (true) {
        // update all satellites and print their positions and velocities
        for (Satellite& s : satellites) {
            s.update(timeStep);
            s.print();
            for (Satellite& neighbor : satellites) {
                double distance = s.distance(neighbor);
                if (&neighbor != &s && distance <= neighborDistance) {
                    std::cout << "Satellite: " << neighbor.getId() << " is neighbor of: " << s.getId() << " with distance: " << distance << std::endl;
                }
            }
        }
        sleep(1);
    }
}