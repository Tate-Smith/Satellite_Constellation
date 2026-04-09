/*
File: Main
Date Created: April 9th, 2026
Last Updated: April 9th, 2026
Author: Tate Smith
Purpose: This file runs the ground control side of the simulation, the ground controls job is to recieve telemtry updates from all the
satellites and every once in a while send course adjustments
*/

#include <iostream>
#include <unistd.h>
#include "../../src/logging/Logger.h"
#include "../../src/concurrency/MessageQueue.h"

void signalHandler(int sig) {
    // a function to handle the SIGINT signal
    exit(0);
}

void messageSatellite() {
    // get the satellite id that the message is to be sent to
    int id;
    std::cout << "Satellite Id? ";
    std::cin >> id;
    // get the message
    std::string str;
    std::cout << "Message: ";
    std::cin >> str;
    // convert that to a struct 
    
}

int main(int argc, char *argv[]) {
    // the input should only be a file containg every satellite and their ip and port information
    if (argc != 2) {
        std::cerr << "Invalid number of arguments" << std::endl;
        return 1;
    }

    std::cout << "Ground Control Up" << std::endl;

    // attempt to connect to all satellites


    // once connected create a logger thread to handle all the data sent from the satellites
    MessageQueue queue;
    Logger logger("Ground_Control_logger.txt", &queue);

    // create another thread to handle sending course adjustments to the satellites


    // handle signal
    signal(SIGINT, signalHandler);

    // loop continuously until user kills the program
    while (true) usleep(10000);  // 0.01 seconds

    return 0;
}