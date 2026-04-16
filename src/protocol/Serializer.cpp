/*
File: Serializer.cpp
Date Created: March 30th, 2026
Last Updated: April 15th, 2026
Purpose: This file implements the serialization functions for converting messages to and from a byte stream
*/

#include "Serializer.h"

std::unique_ptr<Message> decode(const uint8_t* buf, size_t size) {
    // make sure the buffer is big enough
    if (size < sizeof(Header)) throw std::runtime_error("Buffer too small for header.");
    Header head;
    std::memcpy(&head, buf, sizeof(Header));

    if (head.size < sizeof(Header) || head.size > 4096) {
        throw std::runtime_error("Corrupt or invalid message size");
    }

    // figure out which type of message it is
    switch (head.type) {
        case HEARTBEAT:
            return std::make_unique<Heartbeat>(deserializeMessage<Heartbeat>(buf, size));
        case FILE_MSG:
            return std::make_unique<File_Msg>(deserializeMessage<File_Msg>(buf, size));
        case ACK:
            return std::make_unique<Ack>(deserializeMessage<Ack>(buf, size));
        default:
            throw std::runtime_error("Unknown message type");
    }
}