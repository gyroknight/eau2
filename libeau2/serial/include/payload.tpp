/**
 * @file payload.tpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <cstring>
#include <exception>
#include <iostream>

/**
 * @brief Construct a new Payload:: Payload object with the given value.
 * Same as calling add() with the given value.
 *
 * @tparam T
 * @param value
 */
template <typename T>
inline Payload::Payload(T value) {
    if (!add(value)) throw std::invalid_argument("Invalid Payload");
}

template <typename T>
inline bool Payload::add(T value) {
    if (Serial::isType(value) == Serial::Type::Unknown) {
        std::cerr << "Unsupported Payload\n";
        return false;
    }

    if (_type != Serial::Type::Unknown && Serial::isType(value) != _type) {
        std::cerr << "Payload is already set\n";
        return false;
    }

    if (_type == Serial::Type::Unknown) _type = Serial::isType(value);

    uint8_t* valueBytes = reinterpret_cast<uint8_t*>(&value);

    for (size_t ii = 0; ii < sizeof(T); ii++) _data.push_back(valueBytes[ii]);

    return true;
}

template <typename T>
inline bool Payload::add(ColPtr<T> value) {
    return add(std::dynamic_pointer_cast<ColumnInterface>(value));
}