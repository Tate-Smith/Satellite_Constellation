// The .h file for the terminal screen

#ifndef TERMINAL_H
#define TERMINAL_H

#include <vector>
#include <unordered_map>
#include <iostream>
#include <regex>
#include <thread>
#include <ncurses.h>
#include <cassert>
#include "../datastorage/SatelliteData.h"
#include "../../concurrency/MessageQueue.h"
#include "Parser.h"

class Terminal {
    private:
        std::unordered_map<int, std::unique_ptr<SatelliteData>> sats; // stores all the satellite data
        MessageQueue<SatOutput> *outputQueue; // the queue with all the filedumps
        MessageQueue<CommandInput> *inputQueue; // the queue to send all the commands
        Parser parser; // parses the input into CommandInputs
        std::atomic<bool> *running;
        std::thread output_queue_thread;

        void printAllSats(); // private helper that prints out all the satellites stats
        void runQueueThread(std::mutex& mtx); // private helper to print out the satellite datas
        bool checkRegex(const std::string &input, std::string *errorMsg); // private helper to check the validity of the input
        void initInput(const std::string &input, std::string *errorMsg); // private helper to intialize the input section

    public:
        Terminal(MessageQueue<SatOutput> *outputQueue, MessageQueue<CommandInput> *inputQueue, std::atomic<bool> *running);
        void addSat(std::unique_ptr<SatelliteData> sat); // adds a satellite to the vector
        void run(); // this function is constantly checking if the user has input, or there is output to display on the screen
};

#endif