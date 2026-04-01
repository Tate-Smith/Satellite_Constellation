// the .h file for the socketSelector class

#include <vector>

class SocketSelector {
    private:
        std::vector<int> socketFds;

    public:
        void addSocket(int socketFd); // adds a socket file descriptor to the list
        std::vector<int> wait(); // waits and returns a list of all the sockets with data
};