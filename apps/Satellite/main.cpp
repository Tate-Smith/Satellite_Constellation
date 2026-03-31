/*
File: Main
Date Created: March 25th, 2026
Last Updated: March 31st, 2026
Purpose: This file runs the simulation with the time step, and it updates the satellite accordingly
*/

#include "../../src/core/Satellite.h"
#include "../../src/core/Simulation.h"
#include "../../src/network/NetworkManager.h"
#include <unistd.h>
using namespace std;

int main(int argc, char* argv[]) {
    std::cout << "STARTED" << std::endl;
    // make sure valid number of arguments are provided
    if (argc != 10) {
        cout << "Usage: " << argv[0] << " id x y z vx vy vz <my_port> <ip>" << endl;
        return 1;
    }

    // create a satellite with the provided arguments
    Satellite satellite(stoi(argv[1]), stod(argv[2]), stod(argv[3]), stod(argv[4]), stod(argv[5]), stod(argv[6]), stod(argv[7]));

    // add connections to peers
    if (stoi(argv[8]) == 5000) {
        satellite.connectToPeer(argv[9], 5001, 2);
        satellite.connectToPeer(argv[9], 5002, 3);
    }
    else if (stoi(argv[8]) == 5001) {
        satellite.connectToPeer(argv[9], 5000, 1);
        satellite.connectToPeer(argv[9], 5002, 3);
    }
    else if (stoi(argv[8]) == 5002) {
        satellite.connectToPeer(argv[9], 5000, 1);
        satellite.connectToPeer(argv[9], 5001, 2);
    }

    // create a simulation with a time step of 1 second
    Simulation sim(1, satellite);

    // create a network manager and start the server on the provided port
    NetworkManager networkManager;
    networkManager.startServer(stoi(argv[8]));

    // run the simulation
    sim.run(networkManager);

    return 0;
}