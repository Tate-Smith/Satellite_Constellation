// the .h file to represent a connection to a satellite object for ground control

#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string>
#include "../../../src/protocol/Message.h"
#include "../../../src/protocol/Serializer.h"

enum GCConnectionState {
    CONNECTED,
    DISCONNECTED
};

class Connection {
    private:
        int id;
        int satSocket; // the socket of the satellite to send data to
        int port; // the satellites port
        std::string ip; // ip address of the satellite
        sockaddr_in peerAddr;
        GCConnectionState state; // what state the peer is in currently according to this satellite
        time_t lastHeartbeat; // last time it sent a heartbeat message
        time_t lastReconnect; // last time it tried connecting
        int retryCounter; // amount of connects its tried
        int gcPort; // gorund controls port its listening on

    public:
        Connection(int id, int port, std::string ip, int gcPort); // constructor with the satellites listening port
        void connect(); // function to connect to the satellite
        void sendMessage(const Message &messsage) const; // function to send messages to the satellite
        GCConnectionState getState(); // get what the current state of the peer is
        void disconnect(); // disconnect from the given satellite
        void heartbeat(); // updates when the last heart beat was
        void reconnect(); // try to reconnect to the satellite
        bool isTimedOut() const; // checks if the satellite has timed out
        void markConnected(); // sets the connection to connected
};

#endif