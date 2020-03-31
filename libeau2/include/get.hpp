#pragma once

#include <cstdint>

#include "key.hpp"
#include "message.hpp"

// Get data from another KV store, ranging from entire DataFrames to single
// values
class Get : public Message {
   private:
    Key __key;
    uint64_t __colIdx;
    uint64_t __rowIdx;

   public:
    Get(uint64_t sender, uint64_t target, const Key& key,
        uint64_t colIdx = UINT64_MAX, uint64_t rowIdx = UINT64_MAX);
    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};