/*
File: Message.h
Date Created: March 30th, 2026
Last Updated: March 30th, 2026
Purpose: This file is the header file for the message struct, which is used for communication between satellites in the network. 
It defines the structure of a message, including its type, sender ID, and content.
*/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <ctime>

enum MessageType : uint8_t {
    HEARTBEAT,
    STATUS_UPDATE,
    ACK
};

struct Message {
    MessageType type;
    uint32_t senderId;
};

struct Heartbeat : Message {
    time_t timestamp;
    bool alive;
};

struct Status_Message : Message {
    int x;
    int y;
    int z;
    int vx;
    int vy;
    int vz;
};

struct Ack : Message {
    bool received;
};

#endif