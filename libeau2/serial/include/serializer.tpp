#pragma once

#include <cstring>

template <typename T>
inline Serializer& Serializer::add(T value) {
    if (Serial::canSerialize(value)) addBytes(&value, sizeof(T));
    return *this;
}

template <>
inline Serializer& Serializer::add(const char* value) {
    addBytes(const_cast<char*>(value), strlen(value) + 1);
    return *this;
}

template <>
inline Serializer& Serializer::add(const std::string& value) {
    return add(value.c_str());
}

template <>
inline Serializer& Serializer::add(const Key& value) {
    Payload payload(value);
    payload.serialize(*this);
    return *this;
}

template <typename T>
inline Serializer& Serializer::add(std::shared_ptr<Column<T>> col) {
    Payload payload(col);
    payload.serialize(*this);
    return *this;
}

template <>
inline Serializer& Serializer::add(std::shared_ptr<DataFrame> df) {
    Payload payload(df);
    payload.serialize(*this);
    return *this;
}

template <typename T>
inline Serializer& Serializer::add(std::vector<T> vector) {
    for (T& item : vector) add(item);
    return *this;
}