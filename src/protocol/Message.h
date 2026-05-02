/*
File: Message.h
Date Created: March 30th, 2026
Last Updated: May 2nd, 2026
Purpose: This file is the header file for the message struct, which is used for communication between satellites in the network. 
It defines the structure of a message, including its type, sender ID, and content.
*/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include "ProtocolUtils.h"

enum MessageType : uint8_t {
    HEARTBEAT,
    FILE_MSG,
    ACK,
    COMMAND
};

#pragma pack(push, 1)
struct Header {
    uint8_t type;
    uint32_t size;
};
#pragma pack(pop)

struct Message {
    Header header;
    int32_t senderId;
    int32_t senderPort;

    virtual ~Message() = default;
    virtual std::vector<uint8_t> serialize() const = 0;
};

struct Heartbeat : Message {
    bool alive;

    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> buffer;
        appendBytes(buffer, &header, sizeof(header));
        appendBytes(buffer, &senderId, sizeof(senderId));
        appendBytes(buffer, &senderPort, sizeof(senderPort));
        appendBytes(buffer, &alive, sizeof(alive));
        return buffer;
    }
};

struct File_Msg : Message {
    char data[1024];
    int16_t len;
    bool last;

    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> buffer;
        appendBytes(buffer, &header, sizeof(header));
        appendBytes(buffer, &senderId, sizeof(senderId));
        appendBytes(buffer, &senderPort, sizeof(senderPort));
        appendBytes(buffer, &data, sizeof(data));
        appendBytes(buffer, &len, sizeof(len));
        appendBytes(buffer, &last, sizeof(last));
        return buffer;
    }
};

struct Ack : Message {
    bool received;

    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> buffer;
        appendBytes(buffer, &header, sizeof(header));
        appendBytes(buffer, &senderId, sizeof(senderId));
        appendBytes(buffer, &senderPort, sizeof(senderPort));
        appendBytes(buffer, &received, sizeof(received));
        return buffer;
    }
};

struct Command : Message {
    double x, y, z, vx, vy, vz; 
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> buffer;
        appendBytes(buffer, &header, sizeof(header));
        appendBytes(buffer, &senderId, sizeof(senderId));
        appendBytes(buffer, &senderPort, sizeof(senderPort));
        appendBytes(buffer, &x, sizeof(x));
        appendBytes(buffer, &y, sizeof(y));
        appendBytes(buffer, &z, sizeof(z));
        appendBytes(buffer, &vx, sizeof(vx));
        appendBytes(buffer, &vy, sizeof(vy));
        appendBytes(buffer, &vz, sizeof(vz));
        return buffer;
    }
};

#endif