// the .h file for the ConnectionHandler class, which owns and manages all the peer connections in the network

#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include "PeerConnection.h"
#include <unordered_map>

class ConnectionHandler {
    private:
        std::unordered_map<int, PeerConnection> connections;

    public:
        void addIncomingConnection(int socket, const std::string& ip, int peerId);
        void addOutgoingConnection(int peerId, const std::string& ip, int port);
        void update();  // drives reconnection logic
        void removeConnection(int peerId);
        PeerConnection* getConnection(int peerId); // non-owning, may return nullptr
        void sendMessageToPeer(int peerId, const Message& message);
        void broadcastMessage(const Message& message);
        void reconnectPeer(int peerId);
};

#endif