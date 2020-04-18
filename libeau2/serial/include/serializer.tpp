/**
 * @file serializer.tpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <cstring>

template <typename T>
inline Serializer& Serializer::add(T value) {
    if (Serial::canSerialize(value)) addBytes(&value, sizeof(T));
    return *this;
}

template <typename T>
inline Serializer& Serializer::add(ColPtr<T> col) {
    return add(std::dynamic_pointer_cast<ColumnInterface>(col));
}

template <typename T>
inline Serializer& Serializer::add(std::vector<T> vector) {
    for (T& item : vector) add(item);
    return *this;
}