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
    HEARTBEAT = 1,
    FILE_MSG = 2,
    ACK = 3
};

struct Header {
    MessageType type;
    uint16_t size;
};

struct Message {
    Header header;
    int32_t senderId;
};

struct Heartbeat : Message {
    int64_t timestamp;
    bool alive;
};

struct File_Msg : Message {
    uint16_t len;
    char data[4096];
};

struct Ack : Message {
    bool received;
};

#endif