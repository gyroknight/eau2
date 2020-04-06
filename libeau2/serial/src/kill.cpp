#include "kill.hpp"

Kill::Kill(uint64_t sender, uint64_t target)
    : Message(MsgKind::Kill, sender, target, Message::getNextID()) {}

std::unique_ptr<std::vector<uint8_t>> Kill::serialize() {
    Serializer ss;
    setupCmdHdr(ss);
    return ss.generate();
}