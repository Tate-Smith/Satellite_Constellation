// .h file for the NetworkManager class, which handles all network communication between satellites

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <string>
#include <netinet/in.h>
#include "../protocol/Message.h"
#include "ConnectionHandler.h"
#include "../concurrency/MessageQueue.h"

class NetworkManager {
    int serverSocket;
    sockaddr_in serverAddr;
    MessageQueue *queue;

    public:
        NetworkManager(MessageQueue * queue); // constructor to initialize the queue
        void startServer(int port); // starts a server on a given port
        void acceptConnections(ConnectionHandler *handler); // accepts connections from peers to this port
};

#endif