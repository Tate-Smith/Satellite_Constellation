// the .h file for a message queue object

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include "../../apps/GroundControl/output/OutputMessages.h"

template<typename T>
class MessageQueue {
    private:
        std::queue<T> messageQueue;
        std::mutex queueMutex;
        std::condition_variable cv; // condition variable to signal pop that there are elements in the queue
        bool stopped;

    public:
        T pop(); // this pops the first element of the queue and returns it, waits till it contains messages
        void pushBack(const T &message); // this pushes a new string to the back of the queue
        void shutdown();
};

#endif