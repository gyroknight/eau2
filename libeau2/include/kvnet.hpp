#pragma once

#include <memory>

#include "message.hpp"

class KVNet {
   public:
    virtual size_t registerNode() = 0;
    virtual void send(std::shared_ptr<Message> msg) = 0;
    virtual std::shared_ptr<Message> receive(size_t idx) = 0;
};