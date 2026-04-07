/*
File: SocketSelector.cpp
Date Created: April 1st, 2026
Last Updated: April 1st, 2026
Author: Tate Smith
Purpose: This file implements the .h file for the SocketSelector, it wraps the poll() method, and figures out
when the socket has data to read in
*/

#include "SocketSelector.h"
#include <poll.h>

void SocketSelector::addSocket(int socketFd) {
    // make a pollfd variable
    pollfd p{};
    p.fd = socketFd;
    p.events = POLLIN;
    // add the socket file descriptor to the vector
    this->socketFds.push_back(p);
}

std::vector<int> SocketSelector::wait() {
    // get which fds have data, timeout set at 100ms
    int ready = poll(socketFds.data(), socketFds.size(), 100);

    std::vector<int> readyFds;

    // if retval was 0 or lower, error or no data in any sockets
    if (ready <= 0) return readyFds;

    // go through all the fds in the list and check if they are available with poll()
    for (const pollfd& p : socketFds) {
        if (p.revents & POLLIN) readyFds.push_back(p.fd);
    }

    return readyFds;
}