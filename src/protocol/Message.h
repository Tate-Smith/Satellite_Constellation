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

enum MessageType : uint8_t {
    HEARTBEAT,
    STATUS_UPDATE
};

struct Message {
    MessageType type;
    uint32_t senderId;
    double x;
    double y;
    double z;
};

#endif