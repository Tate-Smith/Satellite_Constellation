/*
File: Serializer.cpp
Date Created: March 30th, 2026
Last Updated: March 30th, 2026
Purpose: This file implements the serialization functions for converting messages to and from a byte stream
*/

#include "Serializer.h"
#include <cstring>
#include <vector>

std::vector<std::uint8_t> serializeMessage(const Message& msg) {
    // create a byte vector of the size of the message
    std::vector<std::uint8_t> data(sizeof(Message));
    // copy the message struct into the byte vector
    std::memcpy(data.data(), &msg, sizeof(Message));
    // return the byte vector
    return data;
}

Message deserializeMessage(const std::vector<std::uint8_t>& data) {
    // initialize a message struct
    Message msg;
    // copy the byte vector into the given message struct
    std::memcpy(&msg, data.data(), sizeof(Message));
    // return the message
    return msg;
}