/**
 * @file serial.tpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

class DataFrame;
class Key;

namespace Serial {
template <typename T>
inline bool canSerialize(T value) {
    return isType(value) != Type::Unknown;
}

template <typename U>
inline bool canSerialize(Column<U> value) {
    if (value.size() == 0) return false;
    return canSerializeTrivially(value.get(0));
}

template <typename T>
inline bool canSerializeTrivially(T value) {
    Serial::Type valType = isType(value);
    switch (valType) {
        case Type::U8:
        case Type::I8:
        case Type::U16:
        case Type::I16:
        case Type::U32:
        case Type::I32:
        case Type::U64:
        case Type::I64:
        case Type::Float:
        case Type::Double:
        case Type::String:
            return true;
        default:
            return false;
    }
}

template <typename T>
inline Type isType(T item) {
    return Type::Unknown;
}

template <>
inline Type isType(uint8_t item) {
    return Type::U8;
}

template <>
inline Type isType(int8_t item) {
    return Type::I8;
}

template <>
inline Type isType(uint16_t item) {
    return Type::U16;
}

template <>
inline Type isType(int16_t item) {
    return Type::I16;
}

template <>
inline Type isType(uint32_t item) {
    return Type::U32;
}

template <>
inline Type isType(int32_t item) {
    return Type::I32;
}

template <>
inline Type isType(uint64_t item) {
    return Type::U64;
}

template <>
inline Type isType(int64_t item) {
    return Type::I64;
}

template <>
inline Type isType(float item) {
    return Type::Float;
}

template <>
inline Type isType(double item) {
    return Type::Double;
}

template <>
inline Type isType(ExtString item) {
    return Type::String;
}

template <>
inline Type isType(std::string& item) {
    return Type::String;
}

template <>
inline Type isType(const char* item) {
    return Type::String;
}

template <typename U>
inline Type isType(ColPtr<U> item) {
    return Type::Column;
}

template <>
inline Type isType(Key& item) {
    return Type::Key;
}

template <>
inline Type isType(DFPtr item) {
    return Type::DataFrame;
}

template <typename T>
inline Type isColType(ColPtr<T> col) {
    Type colType = Type::Unknown;
    if (col->size() > 0) colType = isType(col->get(0));
    return canSerializeTrivially(col->get(0)) ? colType : Type::Unknown;
}

}  // namespace Serial