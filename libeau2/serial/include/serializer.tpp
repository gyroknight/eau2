#pragma once

#include <cstring>

template <typename T>
inline Serializer& Serializer::add(T value) {
    addBytes(&value, sizeof(T));
    return *this;
}

template <>
inline Serializer& Serializer::add(const char* value) {
    addBytes(const_cast<char*>(value), strlen(value) + 1);
    return *this;
}

template <>
inline Serializer& Serializer::add(std::string& value) {
    return add(value.c_str());
}

template <typename T>
inline Serializer& Serializer::add(std::shared_ptr<T> ptr) {
    return add(*ptr);
}

template <typename T>
inline Serializer& Serializer::add(std::vector<T> vector) {
    for (T& item : vector) add(item);
    return *this;
}