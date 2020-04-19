/**
 * @file get.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "key.hpp"
#include "message.hpp"

// Get data from another KV store, ranging from entire DataFrames to single
// values
class Get : public Message {
   private:
    static std::unique_ptr<Message> deserializeAs(BStreamIter start,
                                                  BStreamIter end);

    friend std::unique_ptr<Message> Message::deserialize(
        std::unique_ptr<std::vector<uint8_t>>);

   protected:
    Key _key;
    uint64_t _colIdx;
    uint64_t _rowIdx;

    Get(MsgKind kind, uint64_t sender, uint64_t target, const Key& key,
        uint64_t colIdx = UINT64_MAX, uint64_t rowIdx = UINT64_MAX);

   public:
    Get(uint64_t sender, uint64_t target, const Key& key,
        uint64_t colIdx = UINT64_MAX, uint64_t rowIdx = UINT64_MAX);

    const Key& key();
    uint64_t colIdx();
    uint64_t rowIdx();

    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};