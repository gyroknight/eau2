#include "nack.hpp"

Nack::Nack(uint64_t sender, uint64_t target, uint64_t id)
    : Message(MsgKind::Nack, sender, target, id) {}
Nack::~Nack() {}

std::unique_ptr<std::vector<uint8_t>> Nack::serialize() {
    Serializer ss;
    setupCmdHdr(ss);
    return ss.generate();
}