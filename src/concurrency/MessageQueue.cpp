/*
File: Satellite
Date Created: March 25th, 2026
Last Updated: April 8th, 2026
Author: Tate Smith
Purpose: This file represents a message queue for multiple different threads to communicate with eachother
*/

#include "MessageQueue.h"

std::string MessageQueue::pop() {
    const std::lock_guard<std::mutex> lock(this->queueMutex);
    // get the fornt element of the queue
    std::string element = this->messageQueue.front();
    // pop that element
    this->messageQueue.pop();
    return element;
}

void MessageQueue::pushBack(const std::string &message) {
    const std::lock_guard<std::mutex> lock(this->queueMutex);
    this->messageQueue.push(message);
}

bool MessageQueue::hasMessages() {
    if (this->messageQueue.size() > 0) return true;
    return false;
}