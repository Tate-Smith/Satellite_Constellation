/*
File: Logger
Date Created: April 7th, 2026
Last Updated: April 21st, 2026
Author: Tate Smith
Purpose: This file is a logger object that logs everything that happens
*/

#include "Logger.h"

Logger::Logger(const std::string &name, MessageQueue *queue) : queue(queue) {
    // open a file in write mode with the given name
    this->file.open(name);
}

Logger::~Logger() {
    this->file.close();
}

void Logger::log() {
    while (true) {
        // block until the queue has messages
        std::string msg = this->queue->pop();
        // get current time stamp
        time_t cur = time(nullptr);
        tm* time = localtime(&cur);
        char timeStamp[19];
        strftime(timeStamp, sizeof(timeStamp), "%Y-%m-%d %H:%M:%S", time);
        // convert the timeStamp to a string
        std::string timeStr;
        for (int i = 0; i < 19; ++i) timeStr += timeStamp[i];
        std::string logMessage = "[" + timeStr + "] : " + msg; 

        // print out the log message
        std::cout << logMessage << std::endl;
            
        // check if the file is open (created successfully)
        if (this->file.is_open()) {
            this->file << logMessage << std::endl;
        }
    }
}