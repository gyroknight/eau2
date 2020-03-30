// lang::Cpp
#include "message.hpp"

Message::Message(MsgKind kind, size_t sender, size_t target, size_t id)
    : __kind(kind), __sender(sender), __target(target), __id(id) {}
Message::~Message() {}

size_t Message::getNextID() { return __nextID++; }

void Message::setupCmdHdr(Serializer& s) {
    s.add(msgKindToValue(__kind)).add(__sender).add(__target).add(__id);
}

uint8_t Message::msgKindToValue(MsgKind kind) {
    switch (kind) {
        case MsgKind::Ack:
            return 0;
        case MsgKind::Reply:
            return 3;
        case MsgKind::Status:
            return 6;
        case MsgKind::Register:
            return 8;
        case MsgKind::Directory:
            return 9;
        default:
            return UINT8_MAX;
    }
}

MsgKind Message::valueToMsgKind(uint8_t value) {
    switch (value) {
        case 0:
            return MsgKind::Ack;
        case 3:
            return MsgKind::Reply;
        case 6:
            return MsgKind::Status;
        case 8:
            return MsgKind::Register;
        case 9:
            return MsgKind::Directory;
        default:
            return MsgKind::Unknown;
    }
}

std::unique_ptr<Message> Message::deserialize(
    std::unique_ptr<std::vector<uint8_t>> bytestream) {
    if (bytestream->size() < Serial::CMD_HDR_SIZE)
        throw std::invalid_argument("Message is too small");
    std::vector<uint8_t>::iterator bytes = bytestream->begin();
    MsgKind kind = valueToMsgKind(*bytes++);
    uint64_t sender = *reinterpret_cast<uint64_t*>(&(*bytes));
    bytes += sizeof(uint64_t);
    uint64_t target = *reinterpret_cast<uint64_t*>(&(*bytes));
    bytes += sizeof(uint64_t);
    uint64_t id = *reinterpret_cast<uint64_t*>(&(*bytes));
    bytes += sizeof(uint64_t);

    switch (kind) {
        case MsgKind::Ack:
            return nullptr;
        case MsgKind::Reply:
            return nullptr;
        case MsgKind::Status:
            return nullptr;
        case MsgKind::Register:
            return nullptr;
        case MsgKind::Directory:
            return nullptr;
        default:
            return nullptr;
    }
}