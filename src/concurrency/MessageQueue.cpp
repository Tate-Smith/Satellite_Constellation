/*
File: MessageQueue.cpp
Date Created: April 1st, 2026
Last Updated: April 1st, 2026
Author: Tate Smith
Purpose: This file implements the .h file for the message queue, it is a thread safe queue that allows for
Message handling between threads
*/

#include "MessageQueue.h"

void MessageQueue::push(const Message& message) {
    // first lock the mutex so no other threads can modify the queue
    std::lock_guard<std::mutex> lock(mutex);
    // then push the message onto the queue
    this->messageQueue.push(message);
    // notify the consumer thread
    this->var.notify_one();
}

Message MessageQueue::waitAndPop() {
    // this method should block until a message is available
    std::unique_lock<std::mutex> lock(mutex);

    // wait until the queue is not empty
    var.wait(lock, [this] {
        return !messageQueue.empty();
    });

    // get the message thats to be popped
    Message message = messageQueue.front();
    messageQueue.pop();

    return message;
} 

bool MessageQueue::tryPop(Message& message) {
    // lock the queue
    std::lock_guard<std::mutex> lock(mutex);
    // return if the queue has no messages
    if(this->messageQueue.empty()) return false;
    // get the reference to the message in the queue
    message = this->messageQueue.front();
    this->messageQueue.pop();
    return true;
}