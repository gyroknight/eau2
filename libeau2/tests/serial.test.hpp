/**
 * @file serial.test.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "serial.hpp"

namespace {

TEST(SerialUnitTests, valueToType) {
    ASSERT_EQ(Serial::Type::U8, Serial::valueToType(0));
    ASSERT_EQ(Serial::Type::I8, Serial::valueToType(1));
    ASSERT_EQ(Serial::Type::U16, Serial::valueToType(2));
    ASSERT_EQ(Serial::Type::I16, Serial::valueToType(3));
    ASSERT_EQ(Serial::Type::U32, Serial::valueToType(4));
    ASSERT_EQ(Serial::Type::I32, Serial::valueToType(5));
    ASSERT_EQ(Serial::Type::U64, Serial::valueToType(6));
    ASSERT_EQ(Serial::Type::I64, Serial::valueToType(7));
    ASSERT_EQ(Serial::Type::Bool, Serial::valueToType(8));
    ASSERT_EQ(Serial::Type::Float, Serial::valueToType(9));
    ASSERT_EQ(Serial::Type::Double, Serial::valueToType(10));
    ASSERT_EQ(Serial::Type::String, Serial::valueToType(11));
    ASSERT_EQ(Serial::Type::Column, Serial::valueToType(12));
    ASSERT_EQ(Serial::Type::Key, Serial::valueToType(13));
    ASSERT_EQ(Serial::Type::DataFrame, Serial::valueToType(14));
    ASSERT_EQ(Serial::Type::Unknown, Serial::valueToType(15));
    ASSERT_EQ(Serial::Type::Unknown, Serial::valueToType(16));
    ASSERT_EQ(Serial::Type::Unknown, Serial::valueToType(17));
    ASSERT_EQ(Serial::Type::Unknown, Serial::valueToType(18));
}

TEST(SerialUnitTests, typeToValue) {
    ASSERT_EQ(0, Serial::typeToValue(Serial::Type::U8));
    ASSERT_EQ(1, Serial::typeToValue(Serial::Type::I8));
    ASSERT_EQ(2, Serial::typeToValue(Serial::Type::U16));
    ASSERT_EQ(3, Serial::typeToValue(Serial::Type::I16));
    ASSERT_EQ(4, Serial::typeToValue(Serial::Type::U32));
    ASSERT_EQ(5, Serial::typeToValue(Serial::Type::I32));
    ASSERT_EQ(6, Serial::typeToValue(Serial::Type::U64));
    ASSERT_EQ(7, Serial::typeToValue(Serial::Type::I64));
    ASSERT_EQ(8, Serial::typeToValue(Serial::Type::Bool));
    ASSERT_EQ(9, Serial::typeToValue(Serial::Type::Float));
    ASSERT_EQ(10, Serial::typeToValue(Serial::Type::Double));
    ASSERT_EQ(11, Serial::typeToValue(Serial::Type::String));
    ASSERT_EQ(12, Serial::typeToValue(Serial::Type::Column));
    ASSERT_EQ(13, Serial::typeToValue(Serial::Type::Key));
    ASSERT_EQ(14, Serial::typeToValue(Serial::Type::DataFrame));
    ASSERT_EQ(UINT8_MAX, Serial::typeToValue(Serial::Type::Unknown));
}

TEST(SerialUnitTests, isType) {
    ASSERT_EQ(Serial::Type::U8, Serial::isType<uint8_t>((uint8_t)128));
    ASSERT_EQ(Serial::Type::I8, Serial::isType<int8_t>((int8_t)128));
    ASSERT_EQ(Serial::Type::U16, Serial::isType<uint16_t>((uint16_t)128));
    ASSERT_EQ(Serial::Type::I16, Serial::isType<int16_t>((int16_t)128));
    ASSERT_EQ(Serial::Type::U32, Serial::isType<uint32_t>((uint32_t)128));
    ASSERT_EQ(Serial::Type::I32, Serial::isType<int32_t>((int32_t)128));
    ASSERT_EQ(Serial::Type::U64, Serial::isType<uint64_t>((uint64_t)128));
    ASSERT_EQ(Serial::Type::I64, Serial::isType<int64_t>((int64_t)128));
    ASSERT_EQ(Serial::Type::Bool, Serial::isType<bool>(true));
    ASSERT_EQ(Serial::Type::Float, Serial::isType<float>(1.0f));
    ASSERT_EQ(Serial::Type::Double, Serial::isType<double>((double)5.0));

    auto extstring = std::make_shared<std::string>("Hello");
    ASSERT_EQ(Serial::Type::String, Serial::isType<ExtString>(extstring));
    ASSERT_EQ(Serial::Type::String, Serial::isType<const char*>("cstring"));
    ASSERT_EQ(Serial::Type::String, Serial::isType<std::string&>(*extstring));

    // ASSERT_EQ(Serial::Type::Column, Serial::isType()); // TODO

    Key k("hi", 4);
    ASSERT_EQ(Serial::Type::Key, Serial::isType<Key>(k));

    // ASSERT_EQ(Serial::Type::DataFrame, Serial::isType()); // TODO

    std::vector v{1, 2, 3};
    ASSERT_EQ(Serial::Type::Unknown, Serial::isType<decltype(v)>(v));
}

}  // namespace