// lang::Cpp
/**
 * @file dataframe.tpp
 * @author Vincent Zhao
 * @brief Template definitions for DataFrame
 */
#pragma once

#include <exception>

#include "column.hpp"
#include "kvstore.hpp"

/** Adds a column this dataframe, updates the schema, the new column
 * is external, and appears as the last column of the dataframe, the
 * name is optional and external. A nullptr colum is undefined. */
template <typename T>
inline void DataFrame::addCol(ColPtr<T> col, ExtString name) {
    if (_local) {
        if (_schema.addCol(*col, name))
            _data.push_back(col);
        else
            throw std::invalid_argument("col");
    } else {
        _addRemoteCol(col);
    }
}

template <typename T>
inline T DataFrame::getVal(size_t col, size_t row) {
    // cast column interface down to correct type
    auto dfcol = std::dynamic_pointer_cast<Column<T>>(_data.at(col));

    if (!dfcol) throw std::runtime_error("Attempted to getVal as wrong type");

    return dfcol->get(row);
}

/** Set the value at the given column and row to the given value.
 * If the column is not  of the right type or the indices are out of
 * bound, the result is undefined. */
template <typename T>
inline void DataFrame::set(size_t col, size_t row, T val) {
    auto dfCol = std::dynamic_pointer_cast<Column<T>>(_data.at(col));
    dfCol->set(row, val);
}

template <typename T>
inline void DataFrame::fromArray(Key* key, KVStore* kv, size_t size, T* array) {
    auto col = std::make_shared<Column<T>>();

    for (size_t ii = 0; ii < size; ii++) {
        col->push_back(array[ii]);
    }

    auto df = std::make_shared<DataFrame>();
    df->addCol(col);

    kv->push(*key, df);
}

template <typename T>
inline void DataFrame::fromScalar(Key* key, KVStore* kv, T value) {
    fromArray(key, kv, 1, &value);
}

// Remote DataFrame directories consist of 2 Columns only, the first
// representing the name of each Key, and the second holding the home index for
// each Key
template <typename T>
inline void DataFrame::_addRemoteCol(ColPtr<T> col) {
    std::cerr << "DataFrame is remote, cannot add addition Columns\n";
}

template <>
void DataFrame::_addRemoteCol(ColPtr<int64_t> col);

template <>
void DataFrame::_addRemoteCol(ColPtr<ExtString> col);
