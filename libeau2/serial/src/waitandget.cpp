/**
 * @file waitandget.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "waitandget.hpp"

#include <iostream>
#include <iterator>

#include "key.hpp"
#include "message.hpp"
#include "payload.hpp"
#include "serial.hpp"
#include "serializer.hpp"

WaitAndGet::WaitAndGet(uint64_t sender, uint64_t target, const Key& key,
                       uint32_t maxDelay, uint64_t colIdx, uint64_t rowIdx)
    : Get(MsgKind::WaitAndGet, sender, target, key, colIdx, rowIdx),
      _maxDelay(maxDelay) {}

/**
 * @brief WaitAndGet is represented as the following serial format
 *
 * Command Header - see Message
 * colIdx - 8 bytes
 * rowIdx - 8 bytes
 * maxDelay - 4 bytes
 * key - see Payload
 *
 * @return std::unique_ptr<std::vector<uint8_t>> Serial bytestream
 */
std::unique_ptr<std::vector<uint8_t>> WaitAndGet::serialize() {
    Serializer ss;

    setupCmdHdr(ss);
    ss.add(colIdx()).add(rowIdx()).add(_maxDelay).add(key());

    return ss.generate();
}

std::unique_ptr<Message> WaitAndGet::deserializeAs(BStreamIter start,
                                                   BStreamIter end) {
    if (std::distance(start, end) <
        static_cast<int>(2 * sizeof(uint64_t) + sizeof(uint32_t))) {
        std::cerr << "WaitAndGet data is too small\n";
        return nullptr;
    }

    uint64_t colIdx = *reinterpret_cast<uint64_t*>(&(*start));
    start += sizeof(uint64_t);
    uint64_t rowIdx = *reinterpret_cast<uint64_t*>(&(*start));
    start += sizeof(uint64_t);
    uint32_t maxDelay = *reinterpret_cast<uint32_t*>(&(*start));
    start += sizeof(uint32_t);

    Payload key;
    start = key.deserialize(start, end);

    if (key.type() != Serial::Type::Key) {
        std::cerr << "Unexpected WaitAndGet Key\n";
        return nullptr;
    }

    return std::make_unique<WaitAndGet>(0, 0, key.asKey(), maxDelay, colIdx,
                                        rowIdx);
}