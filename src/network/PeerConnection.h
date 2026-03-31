// the .h file for the PeerConnection class, which represents a connection to a peer in the network

#ifndef PEER_CONNECTION_H
#define PEER_CONNECTION_H

#include <string>
#include <ctime>
#include "../protocol/Message.h"
#include <netinet/in.h>

enum ConnectionState {
    CONNECTING,
    CONNECTED,
    DISCONNECTED
};

class PeerConnection {
    private:
        int peerId;
        std::string peerIp;
        int peerPort; // the port each peer is listening on
        int peerSocket; // the socket that each peer listens on
        ConnectionState state; // what state the peer is in currently according to this satellite
        time_t lastHeartbeat; // last time it sent a heartbeat message
        time_t lastReconnect; // last time it tried connecting
        int retryCounter; // amount of connects its tried
        bool isOutgoing; // if the peer was connected to by this satellite
        sockaddr_in peerAddr;

    public:
        PeerConnection(int id, const std::string& ip, int port); // constructor
        void connect(); // connect to the given peer
        void disconnect(); // disconnect from the given peer
        void sendMessage(const Message& message); // send a message to this peer
        Message receiveMessage(); // recieve a message from this peer
        void heartbeat(); // updates when the last heart beat was
        void reconnect(); // try to reconnect to the peer
        ConnectionState getState(); // get what the current state of the peer is
        bool getOutgoing(); // get whether this is an outgoing connection or not
        void setOutgoing(bool b); // set this as an outgoing connection
};

#endif