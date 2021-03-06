/**
 * @file nack.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "nack.hpp"

#include "serializer.hpp"

Nack::Nack(uint64_t sender, uint64_t target, uint64_t id)
    : Message(MsgKind::Nack, sender, target, id) {}
Nack::~Nack() {}

/**
 * @brief Nack is represented as the following serial format:
 *
 * Command Header - see Message
 *
 * @return std::unique_ptr<std::vector<uint8_t>>
 */
std::unique_ptr<std::vector<uint8_t>> Nack::serialize() {
    Serializer ss;
    setupCmdHdr(ss);
    return ss.generate();
}