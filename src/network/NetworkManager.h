// .h file for the NetworkManager class, which handles all network communication between satellites

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <string>
#include <netinet/in.h>
#include "../protocol/Message.h"
#include "ConnectionHandler.h"
#include "../concurrency/MessageQueue.h"
#include "../concurrency/SocketSelector.h"

class NetworkManager {
    int serverSocket;
    sockaddr_in serverAddr;
    MessageQueue& queue;
    SocketSelector selector;

    public:
        void startServer(int port, const MessageQueue& queue); // starts a server on a given port
        void acceptConnections(); // accepts connections from peers to this port
};

#endif