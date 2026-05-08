#ifndef PROTOCOLS_UTILS_H
#define PROTOCOLS_UTILS_H

#include <vector>
#include <cstdint>
#include <cassert>

inline void appendBytes(std::vector<uint8_t>& buffer, const void* data, size_t size) {
    /*
    This method takes in a vector reference of uint8s, a const void data pointer, and a size,
    it then inserts the data into the buffer
    */
    assert(size > 0);
    assert(data != nullptr);
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
    buffer.insert(buffer.end(), ptr, ptr + size);
}

#endif