/**
 * @file payload.test.hpp
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

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

    EXPECT_THROW(Payload p2(true), std::invalid_argument);
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

    p.add(exts1); // "abc"
    p.add(exts2); // "def"

    p.serialize(ss);

    auto bytes = ss.generate();

    EXPECT_EQ(Serial::PAYLOAD_HDR_SIZE + 6, bytes->size());
    
}

// test for bool Payload::add(Key value) {

// test for bool Payload::add(DFPtr value) {

// test for void Payload::serialize(Serializer& ss) {

// test for BStreamIter Payload::deserialize(BStreamIter start, BStreamIter end)
// {

// test for Key Payload::asKey() {

// test for DFPtr Payload::asDataFrame() {

// test for void Payload::_setupThisPayload(Serializer& ss, uint64_t remaining)
// {

// test for void Payload::_serializeColumn(Serializer& ss) {

// test for void Payload::_serializeDataFrame(Serializer& ss) {

// test for BStreamIter Payload::_deserializeColumn(uint64_t& payloadsLeft,

// test for BStreamIter Payload::_deserializeDataFrame(uint64_t& payloadsLeft,

// test for void Payload::_unpackAsCol<ExtString>(Payload& colData,

}  // namespace