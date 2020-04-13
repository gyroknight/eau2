/**
 * @file payload.tpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * Lang::Cpp
 */

#pragma once

#include <exception>

template <typename T>
inline Payload::Payload(std::shared_ptr<Column<T>> col)
    : _type(Serial::Type::Column), _ref(col) {
    _addColType(*col);
}

template <typename T>
inline void Payload::_addColType(const Column<T>& col) {
    throw std::invalid_argument("Unknown column type");
}

template <>
inline void Payload::_addColType(const Column<double>& col) {
    _data[0] = static_cast<uint8_t>(Serial::Type::Double);
}

template <>
inline void Payload::_addColType(const Column<ExtString>& col) {
    _data[0] = static_cast<uint8_t>(Serial::Type::String);
}