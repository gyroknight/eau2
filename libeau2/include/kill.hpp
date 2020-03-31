#pragma once

#include "message.hpp"

// Shuts down a KV store when received.
class Kill : public Message {
   public:
    Kill(uint64_t sender, uint64_t target);
    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};