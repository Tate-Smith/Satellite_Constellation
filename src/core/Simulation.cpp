/*
File: Satellite
Date Created: March 25th, 2026
Last Updated: March 25th, 2026
Purpose: This file runs the simulation with the time step, and updates all satellites accordingly
*/

#include "Simulation.h"
#include <unistd.h>
using namespace std;

Simulation::Simulation(const Satellite& s, double timeStep) : s(s), timeStep(timeStep) {}

void Simulation::run() {
    while (true) {
        s.update(timeStep);
        s.print();
        sleep(1);
    }
}