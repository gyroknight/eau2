/**
 * @file message.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "message.hpp"

#include <iostream>
#include <stdexcept>

#include "ack.hpp"
#include "commondefs.hpp"
#include "directory.hpp"
#include "get.hpp"
#include "kill.hpp"
#include "nack.hpp"
#include "put.hpp"
#include "register.hpp"
#include "reply.hpp"
#include "serial.hpp"
#include "serializer.hpp"
#include "waitandget.hpp"

Message::Message(MsgKind kind, size_t sender, size_t target, size_t id)
    : _kind(kind), _sender(sender), _target(target), _id(id) {}
Message::~Message() {}

MsgKind Message::kind() { return _kind; }
uint64_t Message::sender() { return _sender; }
uint64_t Message::target() { return _target; }
uint64_t Message::id() { return _id; }

size_t Message::getNextID() { return _nextID++; }

/**
 * @brief Adds the Message's Command Header to Serializer.
 *
 * The Command Header is represented by the following serial format:
 *
 * kind - 1 byte
 * sender - 8 bytes
 * target - 8 bytes
 * id - 8 bytes
 *
 * @param s
 */
void Message::setupCmdHdr(Serializer& s) {
    s.add(msgKindToValue(_kind)).add(_sender).add(_target).add(_id);
}

uint8_t Message::msgKindToValue(MsgKind kind) {
    switch (kind) {
        case MsgKind::Ack:
            return 0;
        case MsgKind::Nack:
            return 1;
        case MsgKind::Put:
            return 2;
        case MsgKind::Reply:
            return 3;
        case MsgKind::Get:
            return 4;
        case MsgKind::WaitAndGet:
            return 5;
        case MsgKind::Status:
            return 6;
        case MsgKind::Kill:
            return 7;
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
        case 1:
            return MsgKind::Nack;
        case 2:
            return MsgKind::Put;
        case 3:
            return MsgKind::Reply;
        case 4:
            return MsgKind::Get;
        case 5:
            return MsgKind::WaitAndGet;
        case 6:
            return MsgKind::Status;
        case 7:
            return MsgKind::Kill;
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
    BStreamIter bytes = bytestream->begin();
    MsgKind kind = valueToMsgKind(*bytes++);
    uint64_t sender = *reinterpret_cast<uint64_t*>(&(*bytes));
    bytes += sizeof(uint64_t);
    uint64_t target = *reinterpret_cast<uint64_t*>(&(*bytes));
    bytes += sizeof(uint64_t);
    uint64_t id = *reinterpret_cast<uint64_t*>(&(*bytes));
    bytes += sizeof(uint64_t);

    BStreamIter bytesEnd = bytestream->end();

    std::unique_ptr<Message> msg;

    switch (kind) {
        case MsgKind::Ack:
            return std::make_unique<Ack>(sender, target, id);
        case MsgKind::Nack:
            return std::make_unique<Nack>(sender, target, id);
        case MsgKind::Put:
            msg = Put::deserializeAs(bytes, bytesEnd);
            break;
        case MsgKind::Reply:
            msg = Reply::deserializeAs(bytes, bytesEnd);
            break;
        case MsgKind::Get:
            msg = Get::deserializeAs(bytes, bytesEnd);
            break;
        case MsgKind::WaitAndGet:
            msg = WaitAndGet::deserializeAs(bytes, bytesEnd);
            break;
        case MsgKind::Kill:
            return std::make_unique<Kill>(sender, target);
        case MsgKind::Register:
            msg = Register::deserializeAs(bytes, bytesEnd);
            break;
        case MsgKind::Directory:
            msg = Directory::deserializeAs(bytes, bytesEnd);
            break;
        default:
            std::cerr << "Unknown Message type\n";
            return nullptr;
    }

    if (!msg) {
        std::cerr << "Failed to deserialize Message\n";
        return nullptr;
    }

    msg->_sender = sender;
    msg->_target = target;
    msg->_id = id;

    return msg;
}