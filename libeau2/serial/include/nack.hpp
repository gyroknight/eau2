/**
 * @file nack.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "message.hpp"

class Nack : public Message {
   public:
    Nack(uint64_t sender, uint64_t target, uint64_t id);
    ~Nack();

    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};