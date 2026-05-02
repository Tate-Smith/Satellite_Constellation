/*
File: Logger
Date Created: April 7th, 2026
Last Updated: May 1st, 2026
Author: Tate Smith
Purpose: This file is a logger object that logs everything that happens
*/

#include "Logger.h"

Logger::Logger(const std::string &name, MessageQueue<std::string> *queue) : queue(queue), fileName(name) {
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
        tm timeInfo;
        localtime_r(&cur, &timeInfo);
        char timeStamp[19];
        strftime(timeStamp, sizeof(timeStamp), "%Y-%m-%d %H:%M:%S", &timeInfo);
        // convert the timeStamp to a string
        std::string timeStr;
        for (int i = 0; i < 19; ++i) timeStr += timeStamp[i];
        std::string logMessage = "[" + timeStr + "] : " + msg; 

        std::lock_guard<std::mutex> lock(mtx);
        // print out the log message
        // std::cout << logMessage << std::endl;
            
        // check if the file is open (created successfully)
        if (this->file.is_open()) {
            this->file << logMessage << std::endl;
        }
    }
}

void Logger::clearFile() {
    std::lock_guard<std::mutex> lock(mtx);
    this->file.close();
    this->file.open(this->fileName, std::ios::trunc);
}