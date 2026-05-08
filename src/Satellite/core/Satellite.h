// .h file for Satellite class, defines the class and its functions

#ifndef SATELLITE_H
#define SATELLITE_H
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <cassert>
#include "../../protocol/Message.h"
#include "../network/ConnectionHandler.h"
#include "../../concurrency/MessageQueue.h"
#include "../../config/Config.h"

class Satellite {
    private:
        ConnectionHandler handler; // the satellite owns this handler
        uint32_t id;
        double x, y, z;
        double vx, vy, vz;
        MessageQueue<std::string> *loggerQueue;
        int port; // the port that this satellite is listening on
        bool waitingForAck = false; // whether it needs an acknowledgement sent or not
        std::string filename; // loger file

    public:
        // constructor
        Satellite(uint32_t id, double x, double y, double z, double vx, double vy, double vz, int port, MessageQueue<std::string> *loggerQueue);

        // getters
        uint32_t getId() const;

        double getX() const;

        double getY() const;

        double getZ() const;

        int getPort();

        // connect to a given peer
        void connectToPeer(const std::string& ip, int peerPort, uint32_t peerId);

        // update function
        void update(double dt);

        // print function, gets the id and position and velocity
        void print();

        // create a heartbeat message for this satellite
        Heartbeat createHeartbeatMessage() const;

        std::vector<File_Msg> createDataDump();

        // get the connection handler ptr
        ConnectionHandler* getConnectionHandler();

        // to determine if the ground control succesfully recieved the message
        void setWaitingForAck(bool b);

        bool getWaitingForAck();

        // handles a command from the user
        void handleCommand(const Command &cmd);
};

#endif