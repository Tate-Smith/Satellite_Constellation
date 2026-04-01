// .h for the Message Queue class

#include <mutex>
#include <queue>
#include "../protocol/Message.h"

class MessageQueue {
    private:
        std::mutex mutex;
        std::queue<Message> messageQueue;
        std::condition_variable var;

    public:
        void push(const Message& message); // to push messages onto the queue
        Message waitAndPop(); // blocks until a message arrives and pops
        bool tryPop(Message& message); // returns if there is a message availble to pop
};