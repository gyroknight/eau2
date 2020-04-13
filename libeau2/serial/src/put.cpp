#include "put.hpp"

Put::Put(uint64_t sender, const Key& key, std::shared_ptr<DataFrame> value,
         uint64_t colIdx, uint64_t rowIdx)
    : Message(MsgKind::Put, sender, key.home(), Message::getNextID()),
      _key(key),
      _value(value),
      _colIdx(colIdx),
      _rowIdx(rowIdx) {}

const Key& Put::key() { return _key; }
std::shared_ptr<DataFrame> Put::value() { return _value; }

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