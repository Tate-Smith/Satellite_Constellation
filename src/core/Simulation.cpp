/*
File: Satellite
Date Created: March 25th, 2026
Last Updated: March 25th, 2026
Purpose: This file runs the simulation with the time step, and updates all satellites accordingly
*/

#include "Simulation.h"
#include <unistd.h>
using namespace std;

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
                if (&neighbor != &s && s.distance(neighbor) <= distance) {
                    cout << "Satellite: " << neighbor.getId() << " is neighbor of: " << s.getId() << endl;
                }
            }
        }
        sleep(1);
    }
}