// the .h file to represent a connection to a satellite object for ground control

#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string>
#include <cassert>
#include "../../protocol/Message.h"
#include "../../protocol/Serializer.h"
#include "../../concurrency/MessageQueue.h"
#include "../../config/Config.h"

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
        MessageQueue<std::string> *loggerQueue;
        std::vector<std::uint8_t> msg;

    public:
        Connection(int id, int port, const std::string &ip, int gcPort, MessageQueue<std::string> *loggerQueue); // constructor
        void connect(); // function to connect to the satellite
        void sendMessage(const Message &messsage); // function to send messages to the satellite
        GCConnectionState getState(); // get what the current state of the peer is
        void disconnect(); // disconnect from the given satellite
        void heartbeat(); // updates when the last heart beat was
        bool reconnect(); // try to reconnect to the satellite
        bool isTimedOut() const; // checks if the satellite has timed out
        void markConnected(); // sets the connection to connected
        int getId();
        bool isDead(); // returns if the satellite is dead or not
};

#endif