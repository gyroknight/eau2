/**
 * @file reply.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "reply.hpp"

#include <cstdio>
#include <iostream>

#include "payload.hpp"
#include "serial.hpp"
#include "serializer.hpp"

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

std::unique_ptr<Message> Reply::deserializeAs(BStreamIter start,
                                              BStreamIter end) {
    auto payload = std::make_shared<Payload>();
    payload->deserialize(start, end);

    if (payload->type() == Serial::Type::Unknown) {
        std::cerr << "Cannot read Reply payload\n";
        return nullptr;
    }

    auto reply = std::make_unique<Reply>(0, 0, 0);

    reply->_payload = payload;

    return reply;
}