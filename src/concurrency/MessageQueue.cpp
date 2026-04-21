/*
File: Satellite
Date Created: March 25th, 2026
Last Updated: April 21st, 2026
Author: Tate Smith
Purpose: This file represents a message queue for multiple different threads to communicate with eachother
*/

#include "MessageQueue.h"

std::string MessageQueue::pop() {
    std::unique_lock<std::mutex> lock(this->queueMutex);
    // wait until the queue contains messages 
    this->cv.wait(lock, [this] { return !this->messageQueue.empty(); });
    // get the front element of the queue
    std::string element = this->messageQueue.front();
    // pop that element
    this->messageQueue.pop();
    return element;
}

void MessageQueue::pushBack(const std::string &message) {
    {
        const std::lock_guard<std::mutex> lock(this->queueMutex);
        this->messageQueue.push(message);
    }

    // let the logger know there are messages in the queue
    this->cv.notify_one();
}