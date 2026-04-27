/*
File: Message.h
Date Created: March 30th, 2026
Last Updated: April 21st, 2026
Purpose: This file is the header file for the message struct, which is used for communication between satellites in the network. 
It defines the structure of a message, including its type, sender ID, and content.
*/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include "ProtocolUtils.h"

enum MessageType : uint8_t {
    HEARTBEAT = 1,
    FILE_MSG = 2,
    ACK = 3
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

    virtual ~Message() = default;
    virtual std::vector<uint8_t> serialize() const = 0;
};

struct Heartbeat : Message {
    int64_t timestamp;
    bool alive;

    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> buffer;
        appendBytes(buffer, &header, sizeof(header));
        appendBytes(buffer, &senderId, sizeof(senderId));
        appendBytes(buffer, &timestamp, sizeof(timestamp));
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
        appendBytes(buffer, &received, sizeof(received));
        return buffer;
    }
};

#endif