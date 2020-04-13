/**
 * @file waitandget.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * Lang::Cpp
 */

#include "waitandget.hpp"

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