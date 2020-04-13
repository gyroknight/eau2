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
template <>
inline bool canSerialize(uint8_t value) {
    return true;
}

template <>
inline bool canSerialize(int8_t value) {
    return true;
}

template <>
inline bool canSerialize(uint16_t value) {
    return true;
}

template <>
inline bool canSerialize(int16_t value) {
    return true;
}

template <>
inline bool canSerialize(uint32_t value) {
    return true;
}

template <>
inline bool canSerialize(int32_t value) {
    return true;
}

template <>
inline bool canSerialize(uint64_t value) {
    return true;
}

template <>
inline bool canSerialize(int64_t value) {
    return true;
}

template <>
inline bool canSerialize(float value) {
    return true;
}

template <>
inline bool canSerialize(double value) {
    return true;
}

template <>
inline bool canSerialize(std::string& value) {
    return true;
}

template <>
inline bool canSerialize(const char* value) {
    return true;
}

template <>
inline bool canSerialize(Key& value) {
    return true;
}

template <>
inline bool canSerialize(DataFrame& value) {
    return true;
}

template <typename T>
inline bool canSerialize(T value) {
    return false;
}

template <typename T>
inline bool canSerialize(Column<T> value) {
    if (value.size() == 0) return false;
    return canSerialize(value.get(0));
}
}  // namespace Serial