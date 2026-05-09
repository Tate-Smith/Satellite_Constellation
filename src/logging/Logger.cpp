/*
File: Logger
Date Created: April 7th, 2026
Last Updated: May 8th, 2026
Author: Tate Smith
Purpose: This file is a logger object that logs everything to a given file
*/

#include "Logger.h"

Logger::Logger(const std::string &name, MessageQueue<std::string> *queue, std::atomic<bool> *running) : queue(queue), fileName(name), running(running) {
    /*
    This is the constructor which initalizes all the instance variables and opens the file as well
    */
    assert(!name.empty());
    assert(running != nullptr);
    assert(queue != nullptr);
    // open a file in write mode with the given name
    this->file.open(name);
    // check if it failed
    if (!this->file.is_open()) {
        throw std::runtime_error("File failed to open");
    }
}

Logger::~Logger() {
    /*
    The destructor which closes the file on deletion
    */
    assert(file.is_open());
    assert(running != nullptr);
    try {
        this->file.close();
    } catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void Logger::log() {
    /*
    This is the log method which is run by a thread, it waits for messages to be available in the
    queue and then logs them to the file
    */
    assert(queue != nullptr);
    assert(running != nullptr);
    std::string msg;
    std::string timeStr;
    std::string logMessage;
    while (running->load()) {
        // block until the queue has messages
        timeStr.clear();
        msg = queue->pop();
        if (!running->load()) break;
        if (msg.empty()) continue;
        // get current time stamp
        time_t cur = time(nullptr);
        tm timeInfo = {};
        localtime_r(&cur, &timeInfo);
        char timeStamp[19];
        strftime(timeStamp, sizeof(timeStamp), "%Y-%m-%d %H:%M:%S", &timeInfo);
        // convert the timeStamp to a string
        for (int i = 0; i < 19; ++i) timeStr += timeStamp[i];
        logMessage = "[" + timeStr + "] : " + msg; 

        std::lock_guard<std::mutex> lock(mtx);
            
        // check if the file is open (created successfully)
        if (this->file.is_open()) {
            this->file << logMessage << std::endl;
        }
    }
}

void Logger::clearFile() {
    /*
    This method clears the file of everything inside of it so it is empty
    */
    assert(!fileName.empty());
    assert(file.is_open());
    std::lock_guard<std::mutex> lock(mtx);
    try {
        this->file.close();
    } catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    this->file.open(this->fileName, std::ios::trunc);
}
