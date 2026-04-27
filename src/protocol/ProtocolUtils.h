#ifndef PROTOCOLS_UTILS_H
#define PROTOCOLS_UTILS_H

#include <vector>
#include <cstdint>

inline void appendBytes(std::vector<uint8_t>& buffer, const void* data, size_t size) {
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
    buffer.insert(buffer.end(), ptr, ptr + size);
}

#endif