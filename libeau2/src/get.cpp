#include "get.hpp"

Get::Get(uint64_t sender, uint64_t target, const Key& key, uint64_t colIdx,
         uint64_t rowIdx)
    : Message(MsgKind::Get, sender, target, Message::getNextID()),
      __key(key),
      __colIdx(colIdx),
      __rowIdx(rowIdx){};

std::unique_ptr<std::vector<uint8_t>> Get::serialize() {
    Serializer ss;

    setupCmdHdr(ss);
    ss.add(__key).add(__colIdx).add(__rowIdx);

    return ss.generate();
}