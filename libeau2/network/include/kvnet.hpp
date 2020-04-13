#pragma once

#include <memory>

#include "message.hpp"

// Interface for facilitating communication between distributed stores
class KVNet {
   public:
    virtual size_t registerNode(const char* port) = 0;
    virtual void send(std::shared_ptr<Message> msg) = 0;
    virtual std::unique_ptr<Message> receive() = 0;
};