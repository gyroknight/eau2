/**
 * @file serial.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <cstdint>

#include "commondefs.hpp"
namespace Serial {
constexpr size_t CMD_HDR_SIZE = 25;
constexpr size_t PAYLOAD_HDR_SIZE = 9;

enum class Type {
    U8 = 0,
    I8 = 1,
    U16 = 2,
    I16 = 3,
    U32 = 4,
    I32 = 5,
    U64 = 6,
    I64 = 7,
    Float = 8,
    Double = 9,
    String = 10,
    Column = 11,
    Key = 12,
    DataFrame = 13,
    Unknown
};

inline Type valueToType(uint8_t value) {
    switch (value) {
        case static_cast<uint8_t>(Type::U8):
            return Type::U8;
        case static_cast<uint8_t>(Type::I8):
            return Type::I8;
        case static_cast<uint8_t>(Type::U16):
            return Type::U16;
        case static_cast<uint8_t>(Type::I16):
            return Type::I16;
        case static_cast<uint8_t>(Type::U32):
            return Type::U32;
        case static_cast<uint8_t>(Type::I32):
            return Type::I32;
        case static_cast<uint8_t>(Type::U64):
            return Type::U64;
        case static_cast<uint8_t>(Type::I64):
            return Type::I64;
        case static_cast<uint8_t>(Type::Float):
            return Type::Float;
        case static_cast<uint8_t>(Type::Double):
            return Type::Double;
        case static_cast<uint8_t>(Type::String):
            return Type::String;
        case static_cast<uint8_t>(Type::Column):
            return Type::Column;
        case static_cast<uint8_t>(Type::Key):
            return Type::Key;
        case static_cast<uint8_t>(Type::DataFrame):
            return Type::DataFrame;
        default:
            return Type::Unknown;
    }
}

inline uint8_t typeToValue(Type type) {
    switch (type) {
        case Type::U8:
            return static_cast<uint8_t>(Type::U8);
        case Type::I8:
            return static_cast<uint8_t>(Type::I8);
        case Type::U16:
            return static_cast<uint8_t>(Type::U16);
        case Type::I16:
            return static_cast<uint8_t>(Type::I16);
        case Type::U32:
            return static_cast<uint8_t>(Type::U32);
        case Type::I32:
            return static_cast<uint8_t>(Type::I32);
        case Type::U64:
            return static_cast<uint8_t>(Type::U64);
        case Type::I64:
            return static_cast<uint8_t>(Type::I64);
        case Type::Float:
            return static_cast<uint8_t>(Type::Float);
        case Type::Double:
            return static_cast<uint8_t>(Type::Double);
        case Type::String:
            return static_cast<uint8_t>(Type::String);
        case Type::Column:
            return static_cast<uint8_t>(Type::Column);
        case Type::Key:
            return static_cast<uint8_t>(Type::Key);
        case Type::DataFrame:
            return static_cast<uint8_t>(Type::DataFrame);
        default:
            return UINT8_MAX;
    }
}

template <typename T>
inline bool canSerialize(T value);

template <typename T>
inline bool canSerializeTrivially(T value);

template <typename T>
inline Type isType(T item);

template <typename T>
inline Type isColType(ColPtr<T> col);
}  // namespace Serial

#include "serial.tpp"