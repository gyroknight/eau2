/**
 * @file ack.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * Lang::Cpp
 */

#include "ack.hpp"

Ack::Ack(uint64_t sender, uint64_t target, uint64_t id)
    : Message(MsgKind::Ack, sender, target, id) {}
Ack::~Ack() {}

/**
 * @brief Ack is represented as the following serial format:
 *
 * Command Header - see Message
 *
 * @return std::unique_ptr<std::vector<uint8_t>>
 */
std::unique_ptr<std::vector<uint8_t>> Ack::serialize() {
    Serializer ss;
    setupCmdHdr(ss);
    return ss.generate();
}