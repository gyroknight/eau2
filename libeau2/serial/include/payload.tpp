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
    if (_type != Serial::Type::Unknown) {
        std::cerr << "Payload is already set\n";
        return false;
    }

    if (!Serial::canSerialize(value)) {
        std::cerr << "Unsupported Column type\n";
        return false;
    }

    _type = Serial::Type::Column;
    _colType = Serial::isColType(value);
    _ref = value;

    return true;
}

template <>
inline bool Payload::add(const Key& value);

template <typename T>
inline ColPtr<T> Payload::asColumn() {
    if (_type != Serial::Type::Column) {
        std::cerr << "Payload is not a Column\n";
        return nullptr;
    }

    auto col = std::static_pointer_cast<ColumnInterface>(_ref);

    return std::dynamic_pointer_cast<Column<T>>(col);
}

template <typename T>
inline void Payload::_unpackAsCol(Payload& colData) {
    auto col = std::make_shared<Column<T>>();

    if (colData._data.size() % sizeof(T) != 0) {
        std::cerr << "Column data size mismatch\n";
        return;
    }

    size_t numItems = colData._data.size() / sizeof(T);

    T* item = reinterpret_cast<T*>(colData._data.data());

    for (size_t ii = 0; ii < numItems; ii++) {
        col->push_back(*item++);
    }

    _ref = col;
}

template <>
inline void Payload::_unpackAsCol<ExtString>(Payload& colData);