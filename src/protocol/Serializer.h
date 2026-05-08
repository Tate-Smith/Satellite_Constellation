// the .h file for the serializer functions

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "Message.h"
#include <cstring>
#include <memory>
#include <stdexcept>
#include <cassert>

template<typename T>
std::unique_ptr<T> makeMsg(Header head, int id, int port) {
    /*
    This method takes in a header struct, an id and a port and converts them into a message object
    */
    assert(id >= 0);
    assert(port > 0);
    assert(head.size >= 0);
    auto msg = std::make_unique<T>();
    msg->header = head;
    msg->senderId = id;
    msg->senderPort = port;
    return msg;
}

std::unique_ptr<Message> decode(const uint8_t* buf, size_t size); // decodes from a byte buffer to a struct

#endif