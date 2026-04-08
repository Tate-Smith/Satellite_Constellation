// the .h file for a message queue object

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <queue>
#include <string>
#include <mutex>

class MessageQueue {
    private:
        std::queue<std::string> messageQueue;
        std::mutex queueMutex;

    public:
        std::string pop(); // this pops the first element of the queue and returns it
        void pushBack(const std::string &message); // this pushes a new string to the back of the queue
        bool hasMessages(); // this returns true if there are messages in the queue
};

#endif