/*
File: SocketSelector.cpp
Date Created: April 1st, 2026
Last Updated: April 1st, 2026
Author: Tate Smith
Purpose: This file implements the .h file for the SocketSelector, it wraps the select() method, and figures out
which socjets have data read in them
*/

#include "SocketSelector.h"

void SocketSelector::addSocket(int socketFd) {
    // add the socket file descriptor to the vector
    this->socketFds.push_back(socketFd);
}

std::vector<int> SocketSelector::wait() {
    // set a timeout time of 100 ms
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;

    // call the select command
    int ready = select();
}