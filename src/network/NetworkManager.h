#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <string>
#include <netinet/in.h>

class NetworkManager {
    int serverSocket;
    sockaddr_in serverAddr;
    sockaddr_in peerAddr;

    public:
        void startServer(int port);
        void setPeer(std::string ip, int port);
        void sendMessage(std::string message);
        void receiveMessage();
};

#endif