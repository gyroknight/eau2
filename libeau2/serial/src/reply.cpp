// lang::Cpp
#include "reply.hpp"

Reply::Reply(size_t sender, size_t target, size_t id)
    : Message(MsgKind::Reply, sender, target, id) {}

std::shared_ptr<Payload> Reply::payload() { return _payload; }

/**
 * @brief Reply is represented as the following serial format:
 *
 * Command Header - see Message
 * payload - see Payload
 *
 * @return std::unique_ptr<std::vector<uint8_t>>
 */
std::unique_ptr<std::vector<uint8_t>> Reply::serialize() {
    if (!_payload) {
        printf("Empty Reply\n");
        return nullptr;
    }

    Serializer ss;
    setupCmdHdr(ss);
    _payload->serialize(ss);

    return ss.generate();
}