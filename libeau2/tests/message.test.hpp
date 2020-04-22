/**
 * @file payload.test.hpp
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Tests the following Message subclasses (mostly just serialization code)
 *
 * Ack
 * Directory
 * Get
 * Kill
 * Nack
 * Put
 * Register
 * Reply
 * WaitAndGet
 *
 *
 * Lang::Cpp
 */

#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "ack.hpp"
#include "directory.hpp"
#include "get.hpp"
#include "gtest/gtest.h"
#include "kill.hpp"
#include "message.hpp"
#include "nack.hpp"
#include "payload.hpp"
#include "put.hpp"
#include "register.hpp"
#include "reply.hpp"
#include "serializer.hpp"
#include "testutils.hpp"
#include "waitandget.hpp"

namespace {

class MessageTest : public FixtureWithKVStore {
   protected:
};

// test for serialize();
TEST_F(MessageTest, serialize) {
    Ack ack(1, 1, 2);
    auto bytes = ack.serialize();
    EXPECT_EQ(Serial::CMD_HDR_SIZE, bytes->size());

    Directory directory(1, 1, 2);
    bytes = directory.serialize();
    EXPECT_EQ(Serial::CMD_HDR_SIZE + sizeof(uint64_t) * 2, bytes->size());

    Get get(1, 1, *key);
    bytes = get.serialize();
    EXPECT_EQ(Serial::CMD_HDR_SIZE
                  // column/row idx
                  + 2 * sizeof(uint64_t)
                  // payload with key
                  + Serial::PAYLOAD_HDR_SIZE + 10 /* "dataframe\0" */ +
                  sizeof(size_t),
              bytes->size());
    // Kill kill(0, 1, 2);
    // Nack nack(0, 1, 2);
    // Put put(0, 1, 2);
    // Register register(0, 1, 2);
    // Reply reply(0, 1, 2);
    // WaitAndGet waitandget(0, 1, 2);
}

// test for void setupCmdHdr(Serializer& s);
TEST_F(MessageTest, setupCmdHdr) {}

// test for uint8_t msgKindToValue(MsgKind kind);
TEST_F(MessageTest, msgKindToValue) {}

// test for MsgKind valueToMsgKind(uint8_t value);
TEST_F(MessageTest, valueToMsgKind) {}

// test for deserialize(std::unique_ptr<std::vector<uint8_t>> bytestream);
TEST_F(MessageTest, deserialize) {}

}  // namespace