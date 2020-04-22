/**
 * @file serializer.tpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <cstring>

#include "payload.hpp"
#include "serial.hpp"

template <typename T>
inline Serializer& Serializer::add(T value) {
    if (Serial::canSerializeTrivially(value)) {
        addBytes(&value, sizeof(T));
    } else {
        Payload payload(value);
        payload.serialize(*this);
    }

    return *this;
}

template <>
inline Serializer& Serializer::add(const char* value);

template <>
inline Serializer& Serializer::add(const std::string& value);

template <typename T>
inline Serializer& Serializer::add(std::vector<T> vector) {
    for (T& item : vector) add(item);
    return *this;
}