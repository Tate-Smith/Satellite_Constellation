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
#include <thread>

void broadcast(Satellite &satellite) {
    while (true) {
        satellite.getConnectionHandler()->broadcastMessage(satellite.createStatusMessage());
        usleep(5000000);
    }
}

int main(int argc, char* argv[]) {
    // usage: ./satellite id x y z vx vy vz myport ip id:peerIp1:port id:peerIp2:port id:peerIp3:port ...
    std::cout << "STARTED" << std::endl;

    // create a satellite with the provided arguments
    Satellite satellite(std::stoi(argv[1]), std::stod(argv[2]), std::stod(argv[3]), std::stod(argv[4]), std::stod(argv[5]), std::stod(argv[6]),
    std::stod(argv[7]));

    // parse remaining args and connect to them
    for (int i = 10; i < argc; ++i) {
        std::string arg = argv[i];
        // get the ip and port from each arg
        size_t firstColon = arg.find(":");
        size_t secondColon = arg.find(":", firstColon + 1);
        int peerId = std::stoi(arg.substr(0, firstColon));
        std::string ip = arg.substr(firstColon + 1, secondColon - firstColon - 1);
        int port = std::stoi(arg.substr(secondColon + 1));
        // connect to each port
        satellite.connectToPeer(ip, port, peerId);
    }

    // the main sepration of concerns, sim loop, listening for messages, sending messages

    // create a simulation with a time step of 1 second
    Simulation sim(1, satellite);
    // run a simulation thread with the run() func
    std::thread simulationThread(&Simulation::run, &sim);

    // run a thread to listen for messages (network manager)
    MessageQueue queue;
    NetworkManager networkManager(queue);
    // start a server for the network manager
    networkManager.startServer(std::stoi(argv[8]));
    // listen for connections
    std::thread listenerThread(&NetworkManager::acceptConnections, &networkManager);

    // run a thread to send messages (connection handler through satelliet)
    std::thread senderThread(&broadcast, std::ref(satellite));

    return 0;
}