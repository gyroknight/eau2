/**
 * @file waitandget.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "commondefs.hpp"
#include "get.hpp"
#include "message.hpp"

class Key;

class WaitAndGet : public Get {
   private:
    uint32_t _maxDelay;

    static std::unique_ptr<Message> deserializeAs(BStreamIter start,
                                                  BStreamIter end);

    friend std::unique_ptr<Message> Message::deserialize(
        std::unique_ptr<std::vector<uint8_t>>);

   public:
    WaitAndGet(uint64_t sender, uint64_t target, const Key& key,
               uint32_t maxDelay = 5000, uint64_t colIdx = UINT64_MAX,
               uint64_t rowIdx = UINT64_MAX);

    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};