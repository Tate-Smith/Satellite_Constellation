// the .h file for the PeerConnection class, which represents a connection to a peer in the network

#ifndef PEER_CONNECTION_H
#define PEER_CONNECTION_H

#include <string>
#include <ctime>
#include "../protocol/Message.h"

enum ConnectionState {
    CONNECTING,
    CONNECTED,
    DISCONNECTED
};

class PeerConnection {
    private:
        int peerId;
        std::string peerIp;
        int peerPort;
        int peerSocket;
        ConnectionState state;
        time_t lastHeartbeat;
        time_t lastReconnect;
        int retryCounter;
        bool isOutgoing;

    public:
        PeerConnection(int id, const std::string& ip, int port);
        void connect();
        void disconnect();
        void sendMessage(const Message& message);
        std::string receiveMessage();
        void heartbeat();
        void reconnect();
};

#endif