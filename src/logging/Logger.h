// .h file for the logger class

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>

class Logger {
    private:
        std::ofstream file;

    public:
        Logger(const std::string &name); // constructor that takes in a string fro the name of the file
        ~Logger(); // destructor to close the file at the end
        void log(const std::string &message); // a function used to log messages
};

#endif