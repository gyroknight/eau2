#pragma once

#include "message.hpp"

class Nack : public Message {
   public:
    Nack(uint64_t sender, uint64_t target, uint64_t id);
    ~Nack();

    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};