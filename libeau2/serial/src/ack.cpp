#include "ack.hpp"

Ack::Ack(uint64_t sender, uint64_t target, uint64_t id)
    : Message(MsgKind::Ack, sender, target, id) {}
Ack::~Ack() {}

std::unique_ptr<std::vector<uint8_t>> Ack::serialize() {
    Serializer ss;
    setupCmdHdr(ss);
    return ss.generate();
}