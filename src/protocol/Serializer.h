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
#include <iostream>

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

std::unique_ptr<Message> decode(const uint8_t* buf, size_t size);

#endif