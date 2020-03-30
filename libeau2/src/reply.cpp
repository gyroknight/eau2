// lang::Cpp
#include "reply.hpp"

Reply::Reply(size_t sender, size_t target, size_t id)
    : Message(MsgKind::Reply, sender, target, id),
      __type(Serial::Type::Unknown) {}

void Reply::setPayload(double val) {}

void Reply::setPayload(std::shared_ptr<DataFrame> val) {}

std::unique_ptr<std::vector<uint8_t>> Reply::serialize() {
    if (__payloads.empty()) {
        printf("Empty Reply\n");
        return nullptr;
    }

    Serializer ss;
    setupCmdHdr(ss);
    ss.add(Serial::typeToValue(__type)).add(__payloads.size());
    for (std::shared_ptr<Payload>& payload : __payloads) {
        ss.add(payload);
    }

    return ss.generate();
}