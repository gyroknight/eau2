/**
 * @file put.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "put.hpp"

#include "key.hpp"
#include "serializer.hpp"

Put::Put(uint64_t sender, const Key& key, DFPtr value, uint64_t colIdx,
         uint64_t rowIdx)
    : Message(MsgKind::Put, sender, key.home(), Message::getNextID()),
      _key(key),
      _value(value),
      _colIdx(colIdx),
      _rowIdx(rowIdx) {}

const Key& Put::key() { return _key; }
DFPtr Put::value() { return _value; }

/**
 * @brief Put is represented as the following serial format:
 *
 * Command Header - see Message
 * colIdx - 8 bytes
 * rowIdx - 8 bytes
 * key - see Payload
 * value - see Payload
 *
 * @return std::unique_ptr<std::vector<uint8_t>>
 */
std::unique_ptr<std::vector<uint8_t>> Put::serialize() {
    Serializer ss;

    setupCmdHdr(ss);

    ss.add(_colIdx).add(_rowIdx).add(_key).add(_value);

    return ss.generate();
}

std::unique_ptr<Message> Put::deserializeAs(BStreamIter start,
                                            BStreamIter end) {
    if (std::distance(start, end) < sizeof(uint64_t)) {
        std::cerr << "Put data is too small\n";
        return nullptr;
    }

    uint64_t colIdx = *reinterpret_cast<uint64_t*>(&(*start));
    start += sizeof(uint64_t);
    uint64_t rowIdx = *reinterpret_cast<uint64_t*>(&(*start));
    start += sizeof(uint64_t);

    Payload key;
    start = key.deserialize(start, end);

    if (key.type() != Serial::Type::Key) {
        std::cerr << "Unexpected Put Key\n";
        return nullptr;
    }

    Payload df;
    start = df.deserialize(start, end);

    if (df.type() != Serial::Type::DataFrame) {
        std::cerr << "Unexpected Put value\n";
        return nullptr;
    }

    return std::make_unique<Put>(0, key.asKey(), df.asDataFrame(), colIdx,
                                 rowIdx);
}