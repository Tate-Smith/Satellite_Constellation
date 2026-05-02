// the .h file for the serializer functions

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "Message.h"
#include <cstring>
#include <memory>
#include <stdexcept>

std::unique_ptr<Message> decode(const uint8_t* buf, size_t size);

#endif