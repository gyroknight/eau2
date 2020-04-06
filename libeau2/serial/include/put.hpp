#pragma once

#include "dataframe.hpp"
#include "key.hpp"
#include "message.hpp"

// Puts data from one node to another, ranging from entire DFs to single items
class Put : public Message {
   private:
    const Key& _key;
    std::shared_ptr<DataFrame> _value;
    uint64_t _colIdx;
    uint64_t _rowIdx;

   public:
    Put(uint64_t sender, const Key& key, std::shared_ptr<DataFrame> value,
        uint64_t colIdx = UINT64_MAX, uint64_t rowIdx = UINT64_MAX);
    std::unique_ptr<std::vector<uint8_t>> serialize() override;
};