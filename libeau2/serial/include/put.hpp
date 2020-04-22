/**
 * @file put.hpp
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
#include "key.hpp"
#include "message.hpp"

class Key;

// Puts data from one node to another, ranging from entire DFs to single items
class Put : public Message {
   private:
    const Key _key;
    DFPtr _value;
    uint64_t _colIdx;
    uint64_t _rowIdx;

    static std::unique_ptr<Message> deserializeAs(BStreamIter start,
                                                  BStreamIter end);

    friend std::unique_ptr<Message> Message::deserialize(
        std::unique_ptr<std::vector<uint8_t>>);

   public:
    Put(uint64_t sender, const Key& key, DFPtr value,
        uint64_t colIdx = UINT64_MAX, uint64_t rowIdx = UINT64_MAX);

    const Key& key();
    DFPtr value();

    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};