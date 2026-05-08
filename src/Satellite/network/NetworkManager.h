// .h file for the NetworkManager class, which handles all network communication between satellites

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <cassert>
#include "ConnectionHandler.h"
#include "../../concurrency/MessageQueue.h"
#include "../../protocol/Message.h"
#include "../../protocol/Serializer.h"
#include "../core/Satellite.h"
#include "../../logging/Logger.h"
#include "../../config/Config.h"

class NetworkManager {
    private:
        int serverSocket;
        int satId;
        sockaddr_in serverAddr;
        MessageQueue<std::string> *loggerQueue;
        Satellite *self;
        Logger *logger;
        std::atomic<bool> *running;

        void ack(ConnectionHandler *handler);

    public:
        NetworkManager(MessageQueue<std::string> *loggerQueue, Satellite *self, Logger *logger, int satId, std::atomic<bool> *running); // constructor to initialize the queue
        void startServer(int port); // starts a server on a given port
        void acceptConnections(ConnectionHandler *handler); // accepts connections from peers to this port
};

#endif