// .h file for the logger class

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include "../concurrency/MessageQueue.h"

class Logger {
    private:
        std::ofstream file;
        MessageQueue<std::string> *queue;
        std::string fileName;
        std::mutex mtx;

    public:
        Logger(const std::string &name, MessageQueue<std::string> *queue); // constructor that takes in a string for the name of the file, and the messageQueue
        ~Logger(); // destructor to close the file at the end
        void log(); // a function used to log messages
        void clearFile(); // to clear the log file
};

#endif