// the .h file for the socketSelector class

#ifndef SOCKETSELECTOR_H
#define SOCKETSELECTOR_H

#include <vector>
#include <poll.h>

class SocketSelector {
    private:
        std::vector<pollfd> socketFds;

    public:
        void addSocket(int socketFd); // adds a socket file descriptor to the list
        std::vector<int> wait(); // waits and returns a list of all the sockets that sent data
};

#endif