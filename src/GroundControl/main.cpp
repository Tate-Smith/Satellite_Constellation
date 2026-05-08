/*
File: Main
Date Created: April 9th, 2026
Last Updated: May 8th, 2026
Author: Tate Smith
Purpose: This file runs the ground control side of the simulation, the ground controls job is to recieve telemtry updates from all the
satellites and every once in a while send course adjustments. It creates all the satellite data objects and manages all the different
threads for the ground control, and handles a graceful shutdown 
*/

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <thread>
#include <cassert>
#include <csignal>
#include "../../src/logging/Logger.h"
#include "../../src/concurrency/MessageQueue.h"
#include "networking/Connection.h"
#include "networking/Receiver.h"
#include "networking/GCConnectionHandler.h"
#include "output/Terminal.h"
#include "../config/Config.h"

std::atomic<bool> running(true);

void connectToSatellites(const std::string& file, GCConnectionHandler &handler, Terminal &terminal, MessageQueue<std::string> *loggerQueue) {
    /*
    This file takes in a file reference, a handler reference, and a terminal reference, and a logger queue pointer
    it opens the file and connects to all the satellites given in it via the handler, it also populates the terminal 
    with the satellite data objects

    File format:
    ID    IP    PORT
    1 127.0.0.1 5000
    2 127.0.0.1 5001
    3 127.0.0.1 5002
    4 127.0.0.1 5003
    5 127.0.0.1 5004
    */

    assert(!file.empty());
    assert(Config::GC_PORT >= 0);
   
    // open the file
    std::ifstream data(file);
    // make sure it worked
    if (!data) throw std::runtime_error("File failed to open");
    std::string s;
    int i = 0;
    std::vector<std::string> split;

    // loop through all the data in the file
    while (getline(data, s) && i < Config::MAX_CONFIG_LINES) {
        split.clear();
        // split each line into the id, ip and port
        std::stringstream str(s);
        // split the line
        while (getline(str, s, ' ')) split.push_back(s);
        // get the data
        try {
            int id = std::stoi(split[0]);
            std::string ip = split[1];
            int port = std::stoi(split[2]);
            // then connect to satellite
            handler.addConnection(port, ip, id);
            auto sat = (std::make_unique<SatelliteData>(id, &running));
            sat->start();
            terminal.addSat(std::move(sat));
            i++;
        } catch (const std::exception& e) {
            loggerQueue->pushBack("Failed to parse satellite config: " + std::string(e.what()));
            continue;
        }
    }
}

int main(int argc, char *argv[]) {
    /*
    This is the main method, the listener, sender, update and logger threads are started here as well as shutdown,
    and the Connection handler, Logger, Terminal, and Receiver are initialized here aswell
    */
    assert(argc > 1);
    assert(Config::GC_PORT >= 0);
    // the input should only be a file containg every satellite and their ip and port information
    if (argc != 2) {
        std::cerr << "Invalid number of arguments" << std::endl;
        return 1;
    }

    std::cout << "Ground Control Up" << std::endl;

    MessageQueue<std::string> loggerQueue;
    MessageQueue<SatOutput> outputQueue; // to track updates from the reciver thread
    MessageQueue<CommandInput> inputQueue; // to send commands to the connection handler thread

    Terminal terminal(&outputQueue, &inputQueue, &running);
    GCConnectionHandler handler(&loggerQueue, &outputQueue, &inputQueue, &running, Config::GC_PORT);

    // attempt to connect to all satellites
    connectToSatellites(argv[1], std::ref(handler), terminal, &loggerQueue);

    // once connected create a logger thread to handle all the data sent from the satellites
    Logger logger("output/Ground_Control_logger.txt", &loggerQueue, &running);
    loggerQueue.pushBack("Logger started.");
    
    // start a server
    Receiver receiver(Config::GC_PORT, &loggerQueue, &outputQueue, &running);
    receiver.startServer();

    // create a thread to listen for messages
    std::thread listenerThread(&Receiver::listen, &receiver, &handler);

    // thread to handle sending messages to the satellites
    std::thread senderThread(&GCConnectionHandler::sendCommand, std::ref(handler));

    // create an update thread
    std::thread updateThread(&GCConnectionHandler::update, std::ref(handler));

    // run a logger thread
    std::thread loggerThread(&Logger::log, &logger);

    // run the terminal
    terminal.run();

    // close all the queues
    loggerQueue.shutdown();

    // join all the threads
    listenerThread.join();
    senderThread.join();
    updateThread.join();
    loggerThread.join();

    return 0;
}