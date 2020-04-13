/**
 * @file kill.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * Lang::Cpp
 */

#include "kill.hpp"

Kill::Kill(uint64_t sender, uint64_t target)
    : Message(MsgKind::Kill, sender, target, Message::getNextID()) {}

/**
 * @brief Kill is represented as the following serial format:
 *
 * Command Header - see Message
 *
 * @return std::unique_ptr<std::vector<uint8_t>>
 */
std::unique_ptr<std::vector<uint8_t>> Kill::serialize() {
    Serializer ss;
    setupCmdHdr(ss);
    return ss.generate();
}