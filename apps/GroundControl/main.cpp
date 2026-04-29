/*
File: Main
Date Created: April 9th, 2026
Last Updated: April 27th, 2026
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
// #include "../../src/logging/Logger.h"
// #include "../../src/concurrency/MessageQueue.h"
#include "networking/Connection.h"
#include "networking/Receiver.h"
#include "networking/GCConnectionHandler.h"

void signalHandler(int sig) {
    // a function to handle the SIGINT signal
    exit(0);
}

void messageSatellite(const GCConnectionHandler &handler) {
    while (true) {
        // get the satellite id that the message is to be sent to
        int id;
        std::cout << "Satellite Id? ";
        std::cin >> id;
        // make sure it was valid input
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore();
            std::cout << "Invalid input, should be an integer representing a Satellite Id" << std::endl;
            continue;
        }

        // send a basic heartbeat message
        Heartbeat m;
        m.senderId = 0;
        m.header.type = MessageType::HEARTBEAT;
        m.header.size = sizeof(m);
        m.timestamp = time(nullptr);
        m.alive = true;
        handler.sendMessageToSat(id, m);
    }
}

void connectToSatellites(const std::string& file, GCConnectionHandler& handler) {
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
    }
}

int main(int argc, char *argv[]) {
    // the input should only be a file containg every satellite and their ip and port information
    if (argc != 2) {
        std::cerr << "Invalid number of arguments" << std::endl;
        return 1;
    }

    std::cout << "Ground Control Up" << std::endl;

    GCConnectionHandler handler;

    // attempt to connect to all satellites
    connectToSatellites(argv[1], std::ref(handler));

    // handle signal
    signal(SIGINT, signalHandler);

    // once connected create a logger thread to handle all the data sent from the satellites
    // MessageQueue queue;
    // Logger logger("Ground_Control_logger.txt", &queue);
    
    // start a server
    Receiver receiver;
    receiver.startServer();
    // create a thread to listen for messages
    std::thread listenerThread(&Receiver::listen, &receiver, &handler);
    listenerThread.detach();

    // create another thread to handle sending messages to the satellites
    std::thread senderThread(&messageSatellite, std::ref(handler));
    senderThread.detach();

    // loop continuously until user kills the program
    while(true) usleep(10000);  // 0.01 seconds

    return 0;
}