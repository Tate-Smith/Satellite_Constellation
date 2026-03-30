/*
File: Main
Date Created: March 25th, 2026
Last Updated: March 28th, 2026
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
    if (argc != 11) {
        cout << "Usage: " << argv[0] << " id x y z vx vy vz <my_port> <target_ip> <target_port>" << endl;
        return 1;
    }

    // create a satellite with the provided arguments
    Satellite satellite(stoi(argv[1]), stod(argv[2]), stod(argv[3]), stod(argv[4]), stod(argv[5]), stod(argv[6]), stod(argv[7]));

    // create a simulation with a time step of 1 second
    Simulation sim(1, satellite);

    // create a network manager and start the server on the provided port
    NetworkManager networkManager;
    networkManager.startServer(stoi(argv[8]));

    // send and recieve example message to the target peer
    networkManager.setPeer(argv[9], stoi(argv[10]));
    // sleep 5 seconds to allow both server and target peer start
    sleep(5);

    // send a message
    networkManager.sendMessage(satellite.createStatusMessage());
    networkManager.receiveMessage();

    // run the simulation
    sim.run(networkManager);

    return 0;
}