// lang::Cpp
#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>

#include "serial.hpp"
#include "serializer.hpp"

/**
 * @brief Type of message
 * 
 */
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
    MsgKind _kind;     // the message kind
    uint64_t _sender;  // the index of the sender node
    uint64_t _target;  // the index of the receiver node
    uint64_t _id;      // an id t unique within the node
    inline static std::atomic_uint64_t _nextID = 0;

   protected:
    /**
     * @brief Construct a new Message object
     * 
     * @param kind what type of message
     * @param sender node index of the source
     * @param target node index of the receiver
     * @param id message ID
     */
    Message(MsgKind kind, uint64_t sender, uint64_t target, uint64_t id);

   public:
    /**
     * @brief Destroy the Message object
     * 
     */
    virtual ~Message();

    // Getters
    virtual MsgKind kind() final;
    virtual uint64_t sender() final;
    virtual uint64_t target() final;
    virtual uint64_t id() final;

    /**
     * @brief Get the next (sequential) ID
     * 
     * @return size_t the next ID
     */
    static size_t getNextID();

    /**
     * @brief Serialize the message into a byte vector
     * 
     * @return std::unique_ptr<std::vector<uint8_t>> pointer to the byte vector
     */
    virtual std::unique_ptr<std::vector<uint8_t>> serialize() = 0;

    /**
     * @brief Builds the header for the command
     * 
     * @param s serializer to use
     */
    virtual void setupCmdHdr(Serializer& s) final;

    /**
     * @brief Convert the type of message to a byte value for serialization
     * 
     * @param kind the message type
     * @return uint8_t the byte representation
     */
    static uint8_t msgKindToValue(MsgKind kind);

    /**
     * @brief Convert byte value to MsgKind for deserialization
     * 
     * @param value the byte value
     * @return MsgKind the message type
     */
    static MsgKind valueToMsgKind(uint8_t value);

    /**
     * @brief Convert a byte vector to a message
     * 
     * @param bytestream the stream of bytes
     * @return std::unique_ptr<Message> the deserialized message
     */
    static std::unique_ptr<Message> deserialize(
        std::unique_ptr<std::vector<uint8_t>> bytestream);
};