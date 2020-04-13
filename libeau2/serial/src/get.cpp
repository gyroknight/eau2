#include "get.hpp"

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

std::unique_ptr<std::vector<uint8_t>> Get::serialize() {
    Serializer ss;

    setupCmdHdr(ss);
    ss.add(_colIdx).add(_rowIdx).add(_key);

    return ss.generate();
}