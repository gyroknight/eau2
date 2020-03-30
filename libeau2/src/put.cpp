#include "put.hpp"

Put::Put(uint64_t sender, const Key& key, std::shared_ptr<DataFrame> value,
         uint64_t colIdx, uint64_t rowIdx)
    : Message(MsgKind::Put, sender, key.home(), Message::getNextID()),
      __key(key),
      __value(value),
      __colIdx(colIdx),
      __rowIdx(rowIdx) {}

std::unique_ptr<std::vector<uint8_t>> Put::serialize() {
    Serializer ss;

    setupCmdHdr(ss);

    // Add more data here
    ss.add(__key).add(__colIdx).add(__rowIdx);

    return ss.generate();
}