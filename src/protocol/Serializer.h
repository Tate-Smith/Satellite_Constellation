// the .h file for the serializer functions

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "Message.h"
#include <vector>

std::vector<std::uint8_t> serializeMessage(const Message& msg);
Message deserializeMessage(const std::vector<std::uint8_t>& data);

#endif