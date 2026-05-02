/*
File: Serializer.cpp
Date Created: March 30th, 2026
Last Updated: May 2nd, 2026
Purpose: This file implements the serialization functions for converting messages to and from a byte stream
*/

#include "Serializer.h"

std::unique_ptr<Message> decode(const uint8_t* buf, size_t size) {
    // make sure the buffer is big enough
    if (size < sizeof(Header)) throw std::runtime_error("Buffer too small for header.");

    Header head;
    std::memcpy(&head, buf, sizeof(Header));
    size_t offset = sizeof(Header);

    if (head.size < sizeof(Header) || head.size > 4096) {
        throw std::runtime_error("Corrupt or invalid message size");
    }

    int32_t senderId;
    std::memcpy(&senderId, buf + offset, sizeof(senderId));
    offset += sizeof(senderId);

    int32_t senderPort;
    std::memcpy(&senderPort, buf + offset, sizeof(senderPort));
    offset += sizeof(senderPort);

    // figure out which type of message it is
    switch (head.type) {
        case HEARTBEAT: {
            auto msg_heartbeat = std::make_unique<Heartbeat>();
            msg_heartbeat->header = head;
            msg_heartbeat->senderId = senderId;
            msg_heartbeat->senderPort = senderPort;
            std::memcpy(&msg_heartbeat->alive, buf + offset, sizeof(msg_heartbeat->alive));
            return msg_heartbeat;
        }
        case FILE_MSG: {
            auto msg_file = std::make_unique<File_Msg>();
            msg_file->header = head;
            msg_file->senderId = senderId;
            msg_file->senderPort = senderPort;
            std::memcpy(&msg_file->data, buf + offset, sizeof(msg_file->data));
            offset += sizeof(msg_file->data);
            std::memcpy(&msg_file->len, buf + offset, sizeof(msg_file->len));
            offset += sizeof(msg_file->len);
            std::memcpy(&msg_file->last, buf + offset, sizeof(msg_file->last));
            return msg_file;
        }
        case ACK: {
            auto msg_ack = std::make_unique<Ack>();
            msg_ack->header = head;
            msg_ack->senderId = senderId;
            msg_ack->senderPort = senderPort;
            std::memcpy(&msg_ack->received, buf + offset, sizeof(msg_ack->received));
            return msg_ack;
        }
        default:
            throw std::runtime_error("Unknown message type");
    }
}