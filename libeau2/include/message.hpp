// lang::Cpp
#pragma once

#include <atomic>

#include "serial.hpp"

enum class MsgKind {
    Ack,
    Nack,
    Put,

    Reply,
    Get,
    WaitAndGet,
    Status,

    Kill,
    Register,
    Directory,
    Unknown
};

class Message {
   private:
    MsgKind __kind;   // the message kind
    size_t __sender;  // the index of the sender node
    size_t __target;  // the index of the receiver node
    size_t __id;      // an id t unique within the node
    inline static std::atomic_size_t next_id_ = 0;

   public:
    Message(MsgKind kind, size_t sender, size_t target, size_t id)
        : kind_(kind), sender_(sender), target_(target), id_(id);
    virtual ~Message(){};

    static size_t getNextID() { return next_id_++; }

    virtual SerialData* serialize() = 0;

    virtual void setupCmdHdr(Serializer& s) final {
        s.serialize(msgKindToValue(kind_))
            .serialize(sender_)
            .serialize(target_)
            .serialize(id_);
    }

    static uint8_t msgKindToValue(MsgKind kind) {
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

    static MsgKind valueToMsgKind(uint8_t value) {
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

    static Message* deserialize(uint8_t* bytestream) {
        MsgKind kind = valueToMsgKind(bytestream[0]);
        size_t sender =
            *reinterpret_cast<uint64_t*>(bytestream + sizeof(uint8_t));
        size_t target = *reinterpret_cast<uint64_t*>(
            bytestream + sizeof(uint8_t) + sizeof(uint64_t));
        size_t id = *reinterpret_cast<uint64_t*>(bytestream + sizeof(uint8_t) +
                                                 2 * sizeof(uint64_t));

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
};