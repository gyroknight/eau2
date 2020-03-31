// lang::Cpp
#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>

#include "serial.hpp"
#include "serializer.hpp"

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

// Communication messages between KV stores
class Message {
   private:
    MsgKind __kind;     // the message kind
    uint64_t __sender;  // the index of the sender node
    uint64_t __target;  // the index of the receiver node
    uint64_t __id;      // an id t unique within the node
    inline static std::atomic_uint64_t __nextID = 0;

   protected:
    Message(MsgKind kind, uint64_t sender, uint64_t target, uint64_t id);

   public:
    virtual ~Message();

    virtual MsgKind kind() final;
    virtual uint64_t sender() final;
    virtual uint64_t target() final;
    virtual uint64_t id() final;

    static size_t getNextID();

    virtual std::unique_ptr<std::vector<uint8_t>> serialize() = 0;

    virtual void setupCmdHdr(Serializer& s) final;

    static uint8_t msgKindToValue(MsgKind kind);

    static MsgKind valueToMsgKind(uint8_t value);

    static std::unique_ptr<Message> deserialize(
        std::unique_ptr<std::vector<uint8_t>> bytestream);
};