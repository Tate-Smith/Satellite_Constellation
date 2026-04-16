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
        std::optional<std::string> tryPop(); // this pops the first element of the queue and returns it, if it contains messages
        void pushBack(const std::string &message); // this pushes a new string to the back of the queue
};

#endif