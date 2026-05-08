// the .h file for a message queue object

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include "../GroundControl/output/OutputMessages.h"

template<typename T>
class MessageQueue {
    private:
        std::queue<T> messageQueue; // the generic queue that messages are pushed to
        std::mutex queueMutex; // the mutex to lock the queue and prevent race conditions
        std::condition_variable cv; // condition variable to signal the queue
        bool stopped = false; // to track whether the queue has been locked or not

    public:
        T pop(); // this pops the first element of the queue and returns it, waits till it contains messages
        void pushBack(const T &message); // this pushes a new message to the back of the queue
        void pushBack(T &&message); // this pushes a new message to the back of the queue, its overrloaded to handle moving
        void shutdown(); // shutsdown the queu gracefully
};

#endif