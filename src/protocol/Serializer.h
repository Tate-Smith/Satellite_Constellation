// the .h file for the serializer functions

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "Message.h"
#include <vector>
#include <concepts>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <type_traits>

template<typename T>
requires std::derived_from<T, Message>
std::vector<std::uint8_t> serializeMessage(T obj) {
    // make sure T is copyable
    static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable for memcpy serialization");
    // create a byte vector of the size of the message
    std::vector<std::uint8_t> data(sizeof(T));
    // copy the message struct into the byte vector
    std::memcpy(data.data(), &obj, sizeof(T));
    // return the byte vector
    return data;
}

template<typename T>
requires std::derived_from<T, Message>
T deserializeMessage(const uint8_t* buf, size_t size) {
    // make sure T is copyable
    static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable for memcpy serialization");
    if (size < sizeof(T)) throw std::runtime_error("Buffer too small.");

    // initialize a message struct
    T obj;
    // copy the byte vector into the given message struct
    std::memcpy(&obj, buf, sizeof(T));
    // return the message
    return obj;
}

std::unique_ptr<Message> decode(const uint8_t* buf, size_t size) {
    // make sure the buffer is big enough
    if (size < sizeof(Header)) throw std::runtime_error("Buffer too small for header.");
    const Header* head = reinterpret_cast<const Header*>(buf);

    if (size < head->size) throw std::runtime_error("Buffer too small for message.");

    // figure out which type of message it is
    switch (head->type) {
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

#endif