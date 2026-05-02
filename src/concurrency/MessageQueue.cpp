/*
File: MessageQueue
Date Created: March 25th, 2026
Last Updated: May 2nd 2026
Author: Tate Smith
Purpose: This file represents a message queue for multiple different threads to communicate with eachother
*/

#include "MessageQueue.h"

template<typename T>
void MessageQueue<T>::shutdown() {
    std::lock_guard<std::mutex> lock(this->queueMutex);
    this->stopped = true;
    this->cv.notify_all();
}

template<typename T>
T MessageQueue<T>::pop() {
    std::unique_lock<std::mutex> lock(this->queueMutex);
    // wait until the queue contains messages 
    this->cv.wait(lock, [this] { return !this->messageQueue.empty() || this->stopped; });
    if (this->stopped) return T{};
    // get the front element of the queue
    T element = this->messageQueue.front();
    // pop that element
    this->messageQueue.pop();
    return element;
}

template<typename T>
void MessageQueue<T>::pushBack(const T &message) {
    {
        const std::lock_guard<std::mutex> lock(this->queueMutex);
        this->messageQueue.push(message);
    }

    // let the logger know there are messages in the queue
    this->cv.notify_one();
}

template class MessageQueue<std::string>;
template class MessageQueue<SatOutput>;
template class MessageQueue<CommandInput>;