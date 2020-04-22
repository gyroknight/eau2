/**
 * @file payload.test.hpp
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "payload.hpp"
#include "serializer.hpp"
#include "testutils.hpp"

namespace {

class PayloadTest : public FixtureWithSmallDataFrame {
   protected:
    /**
     * @brief manually deserializer
     *
     * @param bytes
     * @param i  the start index
     * @return uint64_t
     */
    uint64_t manual_deserialize_uint64_t(
        std::shared_ptr<std::vector<uint8_t>> bytes, size_t i = 0) {
        auto v = *bytes;
        return static_cast<uint64_t>(v[i]) |
               (static_cast<uint64_t>(v[i + 1]) << 8) |
               (static_cast<uint64_t>(v[i + 2]) << 16) |
               (static_cast<uint64_t>(v[i + 3]) << 24) |
               (static_cast<uint64_t>(v[i + 4]) << 32) |
               (static_cast<uint64_t>(v[i + 5]) << 40) |
               (static_cast<uint64_t>(v[i + 6]) << 48) |
               (static_cast<uint64_t>(v[i + 7]) << 56);
    }
};

// test for Payload::Payload()
TEST_F(PayloadTest, ctor_default) {
    Payload p;
    Serializer ss;

    ASSERT_EQ(Serial::Type::Unknown, p.type());

    ASSERT_THROW(p.serialize(ss), std::runtime_error);
}

// test for Payload(T value);
TEST_F(PayloadTest, ctor_value) {
    Payload p(5.0f);  // implicitly discovered template args is the only way to
                      // call this constructor
    Serializer ss;

    ASSERT_EQ(Serial::Type::Float, p.type());

    p.serialize(ss);

    auto bytes = ss.generate();

    ASSERT_EQ(Serial::PAYLOAD_HDR_SIZE + sizeof(float), bytes->size());

    Payload p2(true);
    ASSERT_EQ(Serial::Type::Bool, p2.type());
}

// test for Payload(T value) with unsupported values
TEST_F(PayloadTest, ctor_value_bad) {
    std::vector v{1, 2, 3};
    ASSERT_THROW(Payload p(v), std::invalid_argument);
}

// test for Serial::Type Payload::type() const
TEST_F(PayloadTest, type) {
    Payload p1(5.0f);
    EXPECT_EQ(Serial::Type::Float, p1.type());

    Payload p2(static_cast<double>(5.0));
    EXPECT_EQ(Serial::Type::Double, p2.type());

    Payload p3(exts1);  // from testutils
    EXPECT_EQ(Serial::Type::String, p3.type());
}

// test for bool Payload::add(ExtString value) {
TEST_F(PayloadTest, add_ExtString) {
    Payload p;
    Serializer ss;

    p.add(exts1);  // "abc"
    p.add(exts2);  // "def"

    p.serialize(ss);

    auto bytes = ss.generate();

    EXPECT_EQ(Serial::PAYLOAD_HDR_SIZE + 6 + 2 /* for null ptrs */,
              bytes->size());
}

// test for bool Payload::add(Key value) {
TEST_F(PayloadTest, add_Key) {
    Key key("home", 1000);

    Payload p;
    Serializer ss;

    p.add(key);

    p.serialize(ss);

    auto bytes = ss.generate();

    EXPECT_EQ(Serial::PAYLOAD_HDR_SIZE + 5 + sizeof(size_t), bytes->size());
}

TEST_F(PayloadTest, add_ColPtrBool) {
    Payload p;
    Serializer ss;

    p.add<bool>(c1);

    EXPECT_EQ(Serial::Type::Column, p.type());

    p.serialize(ss);

    auto bytes = ss.generate();
    EXPECT_EQ(
        Serial::PAYLOAD_HDR_SIZE + Serial::PAYLOAD_HDR_SIZE + 3 * sizeof(bool),
        bytes->size());
}

TEST_F(PayloadTest, add_ColPtrInt) {
    Payload p;
    Serializer ss;

    p.add<int>(c2);

    EXPECT_EQ(Serial::Type::Column, p.type());

    p.serialize(ss);

    auto bytes = ss.generate();
    EXPECT_EQ(
        Serial::PAYLOAD_HDR_SIZE + Serial::PAYLOAD_HDR_SIZE + 3 * sizeof(int),
        bytes->size());
}

TEST_F(PayloadTest, add_ColPtrExtString) {
    Payload p;
    Serializer ss;

    p.add<ExtString>(c3);

    EXPECT_EQ(Serial::Type::Column, p.type());

    p.serialize(ss);

    auto bytes = ss.generate();
    // 3 rows 4 chars each (with null terminator)
    EXPECT_EQ(Serial::PAYLOAD_HDR_SIZE + Serial::PAYLOAD_HDR_SIZE +
                  3 * 4 * sizeof(char),
              bytes->size());
}

TEST_F(PayloadTest, add_ColPtrDouble) {
    Payload p;
    Serializer ss;

    p.add<double>(c4);

    EXPECT_EQ(Serial::Type::Column, p.type());

    p.serialize(ss);

    auto bytes = ss.generate();
    // 3 rows 4 chars each (with null terminator)
    EXPECT_EQ(Serial::PAYLOAD_HDR_SIZE + Serial::PAYLOAD_HDR_SIZE +
                  3 * sizeof(double),
              bytes->size());
}

// test for bool Payload::add(DFPtr value) {
TEST_F(PayloadTest, add_DFPtr) {
    Payload p;
    Serializer ss;

    p.add(df);

    EXPECT_EQ(Serial::Type::DataFrame, p.type());

    p.serialize(ss);

    auto bytes = ss.generate();
    EXPECT_EQ(Serial::PAYLOAD_HDR_SIZE
                  // bool column
                  + Serial::PAYLOAD_HDR_SIZE + Serial::PAYLOAD_HDR_SIZE +
                  3 * sizeof(bool)
                  // int column
                  + Serial::PAYLOAD_HDR_SIZE + Serial::PAYLOAD_HDR_SIZE +
                  3 * sizeof(int)
                  // ExtString column
                  + Serial::PAYLOAD_HDR_SIZE + Serial::PAYLOAD_HDR_SIZE +
                  3 * 4 * sizeof(char)
                  // double column
                  + Serial::PAYLOAD_HDR_SIZE + Serial::PAYLOAD_HDR_SIZE +
                  3 * sizeof(double),
              bytes->size());
}

// test for void Payload::serialize(Serializer& ss) {
TEST_F(PayloadTest, serialize) {
    Payload p;
    Serializer ss;

    ASSERT_TRUE(p.add<uint64_t>(static_cast<uint64_t>(0xDEADBEEF)));
    p.serialize(ss);

    std::shared_ptr<std::vector<uint8_t>> bytes = ss.generate();

    EXPECT_EQ(Serial::PAYLOAD_HDR_SIZE + sizeof(uint64_t), bytes->size());

    // get type
    Serial::Type type = Serial::valueToType(bytes->at(0));
    EXPECT_EQ(Serial::Type::U64, type);

    uint64_t remaining = manual_deserialize_uint64_t(bytes, 1);
    EXPECT_EQ(0, remaining);

    uint64_t size = manual_deserialize_uint64_t(bytes, 1 + 8);
    EXPECT_EQ(sizeof(uint64_t), size);

    uint64_t val = manual_deserialize_uint64_t(bytes, 1 + 8 + 8);
    EXPECT_EQ(0xDEADBEEF, val);
}

TEST_F(PayloadTest, serialize_col) {
    Payload p;
    Serializer ss;

    ASSERT_TRUE(p.add<ExtString>(c3));
    p.serialize(ss);

    std::shared_ptr<std::vector<uint8_t>> bytes = ss.generate();

    EXPECT_EQ(Serial::PAYLOAD_HDR_SIZE * 2 + 3 * 4 * sizeof(char),
              bytes->size());

    // get first string
    unsigned char* a = bytes->data() + 2 * Serial::PAYLOAD_HDR_SIZE;
    unsigned char* b = bytes->data() + 2 * Serial::PAYLOAD_HDR_SIZE + 4;
    unsigned char* c = bytes->data() + 2 * Serial::PAYLOAD_HDR_SIZE + 8;

    ASSERT_STREQ("abc", reinterpret_cast<char*>(a));
    ASSERT_STREQ("def", reinterpret_cast<char*>(b));
    ASSERT_STREQ("ghi", reinterpret_cast<char*>(c)); 
}

TEST_F(PayloadTest, serialize_df) {
    Payload p;
    Serializer ss;

    ASSERT_TRUE(p.add(df));
    p.serialize(ss);

    std::shared_ptr<std::vector<uint8_t>> bytes = ss.generate();
}

// test for BStreamIter Payload::deserialize(BStreamIter start, BStreamIter end)
TEST_F(PayloadTest, deserialize_simple) {
    Payload p;
    Serializer ss;

    ASSERT_TRUE(p.add<uint64_t>(static_cast<uint64_t>(0xDEADBEEF)));
    p.serialize(ss);

    std::shared_ptr<std::vector<uint8_t>> bytes = ss.generate();

    Payload p2;
    Serializer ss2;
    p2.deserialize(bytes->begin(), bytes->end());

    p2.serialize(ss2);

    auto b2 = ss2.generate();

    ASSERT_EQ(p.type(), p2.type());
    ASSERT_EQ(Serial::PAYLOAD_HDR_SIZE + sizeof(uint64_t), b2->size());
}

TEST_F(PayloadTest, deserialize_df) {
    Payload p;
    Serializer ss;

    ASSERT_TRUE(p.add(df));
    p.serialize(ss);
}

// test for Key Payload::asKey()

// test for DFPtr Payload::asDataFrame()
TEST_F(PayloadTest, asDataFrame) {
    Payload p;
    p.add(df);

    DFPtr df2 = p.asDataFrame();

    ASSERT_EQ(df->ncols(), df2->ncols());
    ASSERT_EQ(df->nrows(), df2->nrows());

    ASSERT_EQ(df->getBool(0, 0), df2->getBool(0, 0));
    ASSERT_EQ(df->getInt(1, 1), df2->getInt(1, 1));
    ASSERT_EQ(df->getString(2, 1), df2->getString(2, 1));
    ASSERT_EQ(df->getDouble(3, 2), df2->getDouble(3, 2));
}

}  // namespace