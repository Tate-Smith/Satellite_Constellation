/*
File: Satellite
Date Created: March 25th, 2026
Last Updated: April 8th, 2026
Author: Tate Smith
Purpose: This file represents a message queue for multiple different threads to communicate with eachother
*/

#include "MessageQueue.h"

std::optional<std::string> MessageQueue::tryPop() {
    const std::lock_guard<std::mutex> lock(this->queueMutex);
    // get the front element of the queue
    std::string element = this->messageQueue.front();
    // pop that element
    this->messageQueue.pop();
    return element;
}

void MessageQueue::pushBack(const std::string &message) {
    const std::lock_guard<std::mutex> lock(this->queueMutex);
    this->messageQueue.push(message);
}