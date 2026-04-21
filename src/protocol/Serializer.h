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

inline void appendBytes(std::vector<uint8_t>& buffer, const void* data, size_t size) {
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
    buffer.insert(buffer.end(), ptr, ptr + size);
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