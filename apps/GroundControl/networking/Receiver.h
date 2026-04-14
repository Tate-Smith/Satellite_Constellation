// .h file to deal with recieving messages from satellites

#ifndef RECIEVER_H
#define RECIEVER_H

#include <netinet/in.h>

class Receiver {
    private:
        int serverSocket;
        sockaddr_in serverAddr;

    public:
        void startServer(); // starts a server on port 8000
        void listen(); // listens for incmong messages
};

#endif