/*
File: Main
Date Created: March 25th, 2026
Last Updated: April 21st, 2026
Author: Tate Smith
Purpose: This file runs the simulation with the time step, and it updates the satellite accordingly
*/

#include "../../src/core/Satellite.h"
#include "../../src/core/Simulation.h"
#include "../../src/network/NetworkManager.h"
#include "../../src/logging/Logger.h"
#include "../../src/concurrency/MessageQueue.h"
#include <unistd.h>
#include <thread>
#include <csignal>

void signalHandler(int sig) {
    // a function to handle the SIGINT signal
    exit(0);
}

void broadcast(Satellite &satellite) {
    int i = 0;
    while (true) {
        ConnectionHandler *handler = satellite.getConnectionHandler();
        handler->broadcastMessage(satellite.createHeartbeatMessage());
        usleep(5000000); // 5 seconds
        if (i % 2 == 0) {
            handler->printAllPeers();
            // every 10 seconds send an update to ground control
            if (handler->hasConnection(0)) {
                std::vector<File_Msg> msgs = satellite.createDataDump();
                for (File_Msg m : msgs) handler->sendMessageToPeer(0, m);
            }
        }
        ++i;
    }
}

int main(int argc, char* argv[]) {
    // usage: ./satellite id x y z vx vy vz myport ip id:peerIp1:port id:peerIp2:port id:peerIp3:port ...
    std::cout << "STARTED" << std::endl;

    // create a message queue object for th elogger to recieve info
    MessageQueue queue;
    queue.pushBack("Logger started."); // push message to know that its working

    // create a logger object
    Logger logger("Satellite_" + std::to_string(std::stoi(argv[1])) + "_logger.txt", &queue);

    // create a satellite with the provided arguments
    Satellite satellite(std::stoi(argv[1]), std::stod(argv[2]), std::stod(argv[3]), std::stod(argv[4]), std::stod(argv[5]), std::stod(argv[6]),
    std::stod(argv[7]), std::stod(argv[7]), &queue);

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

    // the main sepration of concerns, sim loop, listening for messages, sending messages, logger

    // handle signal
    signal(SIGINT, signalHandler);

    // create a simulation with a time step of 1 second
    Simulation sim(1, satellite);
    // run a simulation thread with the run() func
    std::thread simulationThread(&Simulation::run, &sim);
    simulationThread.detach();

    // run a thread to listen for messages (network manager)
    NetworkManager networkManager(&queue, std::stoi(argv[1]));
    // start a server for the network manager
    networkManager.startServer(std::stoi(argv[8]));
    // listen for connections
    std::thread listenerThread(&NetworkManager::acceptConnections, &networkManager, satellite.getConnectionHandler());
    listenerThread.detach();

    // run a thread to send messages (connection handler through satellite)
    std::thread senderThread(&broadcast, std::ref(satellite));
    senderThread.detach();

    // run a logger thread
    std::thread loggerThread(&Logger::log, &logger);
    loggerThread.detach();

    // loop continuously until user kills the program
    while (true) usleep(10000);  // 0.01 seconds

    return 0;
}