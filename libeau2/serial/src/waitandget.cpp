#include "waitandget.hpp"

WaitAndGet::WaitAndGet(uint64_t sender, uint64_t target, const Key& key,
                       uint32_t maxDelay, uint64_t colIdx, uint64_t rowIdx)
    : Get(MsgKind::WaitAndGet, sender, target, key, colIdx, rowIdx),
      _maxDelay(maxDelay) {}

std::unique_ptr<std::vector<uint8_t>> WaitAndGet::serialize() {
    Serializer ss;

    setupCmdHdr(ss);
    ss.add(_maxDelay).add(key()).add(colIdx()).add(rowIdx());

    return ss.generate();
}