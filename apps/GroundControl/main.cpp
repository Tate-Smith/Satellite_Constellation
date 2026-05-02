/*
File: Main
Date Created: April 9th, 2026
Last Updated: May 1st, 2026
Author: Tate Smith
Purpose: This file runs the ground control side of the simulation, the ground controls job is to recieve telemtry updates from all the
satellites and every once in a while send course adjustments
*/

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <thread>
#include "../../src/logging/Logger.h"
#include "../../src/concurrency/MessageQueue.h"
#include "networking/Connection.h"
#include "networking/Receiver.h"
#include "networking/GCConnectionHandler.h"
#include "output/Terminal.h"

std::atomic<bool> running(true);

void connectToSatellites(const std::string& file, GCConnectionHandler &handler, Terminal &terminal) {
    // this function goes through the file line by line and trys to connect to each satelite 
    /*
    File format:
    ID    IP    PORT
    1 127.0.0.1 5000
    2 127.0.0.1 5001
    3 127.0.0.1 5002
    4 127.0.0.1 5003
    5 127.0.0.1 5004
    */
   // open the file
   std::ifstream data(file);
   std::string s;

   // loop through all the data in the file
    while (getline(data, s)) {
        // split each line into the id, ip and port
        std::stringstream str(s);
        std::vector<std::string> split;
        // split the line
        while (getline(str, s, ' ')) split.push_back(s);
        // get the data
        int id = std::stoi(split[0]);
        std::string ip = split[1];
        int port = std::stoi(split[2]);

        // then connect to satellite
        handler.addConnection(port, ip, id, 8000);
        auto sat = (std::make_unique<SatelliteData>(id));
        sat->start();
        terminal.addSat(std::move(sat));
    }
}

int main(int argc, char *argv[]) {
    // the input should only be a file containg every satellite and their ip and port information
    if (argc != 2) {
        std::cerr << "Invalid number of arguments" << std::endl;
        return 1;
    }

    std::cout << "Ground Control Up" << std::endl;

    MessageQueue<std::string> loggerQueue;
    MessageQueue<SatOutput> outputQueue; // to track updates from the reciver thread
    MessageQueue<CommandInput> inputQueue; // to send commands to the connection handler thread

    Terminal terminal(&outputQueue, &inputQueue);
    GCConnectionHandler handler(&loggerQueue, &outputQueue, &inputQueue, &running);

    // attempt to connect to all satellites
    connectToSatellites(argv[1], std::ref(handler), terminal);

    // once connected create a logger thread to handle all the data sent from the satellites
    Logger logger("Ground_Control_logger.txt", &loggerQueue, &running);
    loggerQueue.pushBack("Logger started.");
    
    // start a server
    Receiver receiver(&loggerQueue, &outputQueue, &running);
    receiver.startServer();

    // create a thread to listen for messages
    std::thread listenerThread(&Receiver::listen, &receiver, &handler);

    // thread to handle sending messages to the satellites
    std::thread senderThread(&GCConnectionHandler::sendCommands, std::ref(handler));

    // create an update thread
    std::thread updateThread(&GCConnectionHandler::update, std::ref(handler));

    // run a logger thread
    std::thread loggerThread(&Logger::log, &logger);

    // run the terminal
    terminal.run();

    // close all the queues
    running = false;
    loggerQueue.shutdown();
    inputQueue.shutdown();

    // join all the threads
    listenerThread.join();
    senderThread.join();
    updateThread.join();
    loggerThread.join();

    return 0;
}