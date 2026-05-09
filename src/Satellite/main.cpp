/*
File: Main
Date Created: March 25th, 2026
Last Updated: May 8th, 2026
Author: Tate Smith
Purpose: This file runs the datellite process, it creates the satellite object along with its many
threads, and it handles graceful shutdown
*/

#include <unistd.h>
#include <thread>
#include <csignal>
#include "core/Satellite.h"
#include "core/Simulation.h"
#include "network/NetworkManager.h"
#include "../logging/Logger.h"
#include "../concurrency/MessageQueue.h"
#include "../config/Config.h"

std::atomic<bool> running(true);

void signalHandler(int sig) {
    /*
    This method handles the SIGINT signal for shutdown
    */
    assert(sig >= 0);
    exit(0);
}

void broadcast(Satellite &satellite) {
    /*
    This method takes in a satellite reference and broadcasts heartbeat messages every 5 seconds , and every
    10 seconds it prints the status of all its peers
    */
    assert(Config::GC_PORT >= 0);
    assert(running);
    int i = 0;
    ConnectionHandler *handler = satellite.getConnectionHandler();
    assert(handler != nullptr);
    // add the ground control as a connection off the bat
    handler->addConnection(Config::GC_PORT, "127.0.0.1", 0, satellite.getId());
    while (running) {
        handler->broadcastMessage(satellite.createHeartbeatMessage());
        sleep(Config::HEARTBEAT_INTERVAL);
        if (i % Config::DUMP_INTERVAL == 0) {
            handler->printAllPeers();
            // send a downlink every hearbeat interval * dump interval (ie : 5 * 2 = 10 seconds)
            if (handler->hasConnection(0)) {
                std::vector<File_Msg> msgs = satellite.createDataDump();
                for (const File_Msg &m : msgs) {
                    handler->sendMessageToPeer(0, m);
                }
                satellite.setWaitingForAck(true);
            }
        }
        ++i;
    }
}

int main(int argc, char* argv[]) {
    /*
    This is the main method which intializes the logger, listener, sender, and simulation threads, along with creating the 
    satellite object and connecting it to its provided peers
    */
    assert(argc > 1);
    if (argc < 10) std::cerr << "Usage: ./satellite id x y z vx vy vz myport ip id:peerIp1:port id:peerIp2:port id:peerIp3:port ..." << std::endl;
    // usage: ./satellite id x y z vx vy vz myport ip id:peerIp1:port id:peerIp2:port id:peerIp3:port ...
    std::cout << "STARTED" << std::endl;

    // create a message queue object for the logger to recieve info
    MessageQueue<std::string> loggerQueue;
    // create a satellite with the provided arguments
    Satellite satellite(std::stoi(argv[1]), std::stod(argv[2]), std::stod(argv[3]), std::stod(argv[4]), std::stod(argv[5]), std::stod(argv[6]),
    std::stod(argv[7]), std::stod(argv[8]), &loggerQueue);

    std::string arg;
    std::string ip;
    // parse remaining args and connect to them
    for (int i = 10; i < argc; ++i) {
        arg = argv[i];
        // get the ip and port from each arg
        size_t firstColon = arg.find(":");
        size_t secondColon = arg.find(":", firstColon + 1);
        try {
            int peerId = std::stoi(arg.substr(0, firstColon));
            ip = arg.substr(firstColon + 1, secondColon - firstColon - 1);
            int port = std::stoi(arg.substr(secondColon + 1));
            // connect to each port
            satellite.connectToPeer(ip, port, peerId);
        } catch (const std::exception& e) {
            loggerQueue.pushBack("Failed to parse peer argument: " + std::string(e.what()));
            continue;
        }
    }

    // handle signal
    signal(SIGINT, signalHandler);

    loggerQueue.pushBack("Logger started.");
    // create a logger object
    Logger logger("output/Satellite_" + std::to_string(std::stoi(argv[1])) + "_logger.txt", &loggerQueue, &running);

    // run a thread to listen for messages (network manager)
    NetworkManager networkManager(&loggerQueue, &satellite, &logger, std::stoi(argv[1]), &running);
    // start a server for the network manager
    networkManager.startServer(std::stoi(argv[8]));
    // listen for connections
    std::thread listenerThread(&NetworkManager::acceptConnections, &networkManager, satellite.getConnectionHandler());

    // run a thread to send messages (connection handler through satellite)
    std::thread senderThread(&broadcast, std::ref(satellite));

    // create a simulation with a time step
    Simulation sim(Config::TIME_STEP, satellite, &running);
    // run a simulation thread with the run() func
    std::thread simulationThread(&Simulation::run, &sim);

    // run a logger thread
    std::thread loggerThread(&Logger::log, &logger);

    // loop continuously until user kills the program
    while (running) usleep(10000);  // 0.01 seconds
    running = false;
    loggerQueue.shutdown();
    listenerThread.join();
    senderThread.join();
    simulationThread.join();
    loggerThread.join();
    return 0;
}