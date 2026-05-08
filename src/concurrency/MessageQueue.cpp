/*
File: MessageQueue
Date Created: March 25th, 2026
Last Updated: May 8th, 2026
Author: Tate Smith
Purpose: This file represents a thread safe queue wrapper as a message queue, it allows different threads to communicate with eachother, 
by pushing messages onto the queue and popping them off when it contains messages. It is a generic so it can handle multiple different types
*/

#include "MessageQueue.h"
#include <iostream>

template<typename T>
void MessageQueue<T>::shutdown() {
    /*
    This function shutsdown the message queue gracefully, by setting stopped to true, and notifying the condition variable of
    change
    */
    {
        std::lock_guard<std::mutex> lock(this->queueMutex);
        this->stopped = true;
    }
    this->cv.notify_all();
}

template<typename T>
T MessageQueue<T>::pop() {
    /*
    This function waits until there are messages in the queue or until the queue has been shutdown, and then pops the
    message and returns it or shutsdown accordingly
    */
    std::unique_lock<std::mutex> lock(this->queueMutex);
    // wait until the queue contains messages 
    this->cv.wait(lock, [this] { return !this->messageQueue.empty() || this->stopped; });
    if (this->stopped) return T{};
    // get the front element of the queue
    T element = std::move(this->messageQueue.front());
    // pop that element
    this->messageQueue.pop();
    return element;
}

template<typename T>
void MessageQueue<T>::pushBack(const T &message) {
    /*
    This function takes in a const message reference, and pushes to the message to the queue safely,
    it then notifys the condition variable of messages in the queue
    */
    {
        const std::lock_guard<std::mutex> lock(this->queueMutex);
        this->messageQueue.push(message);
    }

    // let the logger know there are messages in the queue
    this->cv.notify_one();
}

template<typename T>
void MessageQueue<T>::pushBack(T &&message) {
    /*
    This function takes in a const message reference, and pushes to the message to the queue safely,
    it then notifys the condition variable of messages in the queue
    */
    {
        const std::lock_guard<std::mutex> lock(this->queueMutex);
        this->messageQueue.push(std::move(message));
    }

    // let the logger know there are messages in the queue
    this->cv.notify_one();
}

template class MessageQueue<std::string>;
template class MessageQueue<SatOutput>;
template class MessageQueue<CommandInput>;