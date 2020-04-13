#pragma once

#include "message.hpp"

class Ack : public Message {
   public:
    Ack(uint64_t sender, uint64_t target, uint64_t id);
    ~Ack();

    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};