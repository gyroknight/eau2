/**
 * @file get.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "get.hpp"

#include <iostream>
#include <iterator>

#include "payload.hpp"
#include "serial.hpp"
#include "serializer.hpp"

Get::Get(MsgKind kind, uint64_t sender, uint64_t target, const Key& key,
         uint64_t colIdx, uint64_t rowIdx)
    : Message(kind, sender, target, Message::getNextID()),
      _key(key),
      _colIdx(colIdx),
      _rowIdx(rowIdx){};

Get::Get(uint64_t sender, uint64_t target, const Key& key, uint64_t colIdx,
         uint64_t rowIdx)
    : Message(MsgKind::Get, sender, target, Message::getNextID()),
      _key(key),
      _colIdx(colIdx),
      _rowIdx(rowIdx){};

const Key& Get::key() { return _key; }
uint64_t Get::colIdx() { return _colIdx; }
uint64_t Get::rowIdx() { return _rowIdx; }

/**
 * @brief Get is represented as the following serial format:
 *
 * Command Header - see Message
 * colIdx - 8 bytes
 * rowIdx - 8 bytes
 * key - see Payload
 *
 * @return std::unique_ptr<std::vector<uint8_t>>
 */
std::unique_ptr<std::vector<uint8_t>> Get::serialize() {
    Serializer ss;

    setupCmdHdr(ss);
    ss.add(_colIdx).add(_rowIdx).add(_key);

    return ss.generate();
}

std::unique_ptr<Message> Get::deserializeAs(BStreamIter start,
                                            BStreamIter end) {
    if (std::distance(start, end) < static_cast<int>(2 * sizeof(uint64_t))) {
        std::cerr << "Get data is too small\n";
        return nullptr;
    }

    uint64_t colIdx = *reinterpret_cast<uint64_t*>(&(*start));
    start += sizeof(uint64_t);
    uint64_t rowIdx = *reinterpret_cast<uint64_t*>(&(*start));
    start += sizeof(uint64_t);

    Payload key;
    start = key.deserialize(start, end);

    if (key.type() != Serial::Type::Key) {
        std::cerr << "Unexpected Get Key\n";
        return nullptr;
    }

    return std::make_unique<Get>(0, 0, key.asKey(), colIdx, rowIdx);
}