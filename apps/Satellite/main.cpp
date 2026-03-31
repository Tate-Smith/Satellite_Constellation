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

int main(int argc, char* argv[]) {
    // usage: ./satellite id x y z vx vy vz myport peerIp1:port peerIp2:port peerIp3:port ...
    std::cout << "STARTED" << std::endl;

    // create a satellite with the provided arguments
    Satellite satellite(std::stoi(argv[1]), std::stod(argv[2]), std::stod(argv[3]), std::stod(argv[4]), std::stod(argv[5]), std::stod(argv[6]),
    std::stod(argv[7]));

    // parse remaining args and connect to them
    for (int i = 10; i < argc; ++i) {
        std::string arg = argv[i];
        // get the ip and port from each arg
        int colon = arg.find(":");
        std::string ip = arg.substr(0, colon);
        int port = std::stoi(arg.substr(colon + 1));
        // connect to each port
        satellite.connectToPeer(ip, port, i - 9);
    }

    // create a simulation with a time step of 1 second
    Simulation sim(1, satellite);

    // create a network manager and start the server on the provided port
    NetworkManager networkManager;
    networkManager.startServer(std::stoi(argv[8]));

    // run the simulation
    sim.run(networkManager);

    return 0;
}