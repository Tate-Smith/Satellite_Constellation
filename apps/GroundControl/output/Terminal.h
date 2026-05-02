// The .h file for the terminal screen

#ifndef TERMINAL_H
#define TERMINAL_H

#include <vector>
#include <unordered_map>
#include <iostream>
#include "../datastorage/SatelliteData.h"
#include "../../../src/concurrency/MessageQueue.h"
#include "Parser.h"

class Terminal {
    private:
        std::unordered_map<int, std::unique_ptr<SatelliteData>> sats;
        MessageQueue<SatOutput> *output_queue;
        MessageQueue<CommandInput> *input_queue;
        Parser parser;

        void printAllSats();

    public:
        Terminal(MessageQueue<SatOutput> *output_queue, MessageQueue<CommandInput> *input_queue);
        void addSat(std::unique_ptr<SatelliteData> sat); // adds a satellite to the vector
        void run(); // this function is constantly checking if the user has input, or there is output to display on the screen
};

#endif