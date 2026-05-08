/*
File: Serializer.cpp
Date Created: March 30th, 2026
Last Updated: May 7th, 2026
Purpose: This file implements a decoding function that allows for the serialization of each specific message type
*/

#include "Serializer.h"
#include <iostream>

std::unique_ptr<Message> decode(const uint8_t* buf, size_t size) {
    /*
    This method takes in a uint8 pointer and a size, it then figures out which message type it is and decodes it and returns it
    */
    assert(buf != nullptr);
    assert(size > 0);
    // make sure the buffer is big enough
    if (size < sizeof(Header)) throw std::runtime_error("Buffer too small for header.");
    Header head;
    std::memcpy(&head, buf, sizeof(Header));
    size_t offset = sizeof(Header);

    if (head.size < sizeof(Header) || head.size > 4096) throw std::runtime_error("Corrupt or invalid message size");

    int32_t senderId;
    std::memcpy(&senderId, buf + offset, sizeof(senderId));
    offset += sizeof(senderId);
    int32_t senderPort;
    std::memcpy(&senderPort, buf + offset, sizeof(senderPort));
    offset += sizeof(senderPort);

    // figure out which type of message it is
    switch (head.type) {
        case HEARTBEAT: {
            auto msg_heartbeat = makeMsg<Heartbeat>(head, senderId, senderPort);
            std::memcpy(&msg_heartbeat->alive, buf + offset, sizeof(msg_heartbeat->alive));
            return msg_heartbeat;
        }
        case FILE_MSG: {
            auto msg_file = makeMsg<File_Msg>(head, senderId, senderPort);
            std::memcpy(&msg_file->data, buf + offset, sizeof(msg_file->data));
            offset += sizeof(msg_file->data);
            std::memcpy(&msg_file->len, buf + offset, sizeof(msg_file->len));
            offset += sizeof(msg_file->len);
            std::memcpy(&msg_file->last, buf + offset, sizeof(msg_file->last));
            return msg_file;
        }
        case ACK: {
            auto msg_ack = makeMsg<Ack>(head, senderId, senderPort);
            std::memcpy(&msg_ack->received, buf + offset, sizeof(msg_ack->received));
            return msg_ack;
        }
        case COMMAND: {
            auto msg_cmd = makeMsg<Command>(head, senderId, senderPort);
            std::memcpy(&msg_cmd->x, buf + offset, sizeof(msg_cmd->x));
            offset += sizeof(msg_cmd->x);
            std::memcpy(&msg_cmd->y, buf + offset, sizeof(msg_cmd->y));
            offset += sizeof(msg_cmd->y);
            std::memcpy(&msg_cmd->z, buf + offset, sizeof(msg_cmd->z));
            offset += sizeof(msg_cmd->z);
            std::memcpy(&msg_cmd->vx, buf + offset, sizeof(msg_cmd->vx));
            offset += sizeof(msg_cmd->vx);
            std::memcpy(&msg_cmd->vy, buf + offset, sizeof(msg_cmd->vy));
            offset += sizeof(msg_cmd->vy);
            std::memcpy(&msg_cmd->vz, buf + offset, sizeof(msg_cmd->vz));
            return msg_cmd;
        }
        default:
            throw std::runtime_error("Unknown message type");
    }
}