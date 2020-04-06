// lang::Cpp
#pragma once

#include <exception>

/** Adds a column this dataframe, updates the schema, the new column
 * is external, and appears as the last column of the dataframe, the
 * name is optional and external. A nullptr colum is undefined. */
template <typename T>
inline void DataFrame::addCol(std::shared_ptr<Column<T>> col, ExtString name) {
    if (_schema.addCol(*col, name))
        _data.push_back(col);
    else
        throw std::invalid_argument("col");
}

template <typename T>
inline T DataFrame::getVal(size_t col, size_t row) {
    // cast column interface down to correct type
    auto dfcol = std::dynamic_pointer_cast<Column<T>>(_data.at(col));

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

    kv->insert(*key, df);
    kv->push(*key, df);
}

template <typename T>
inline void DataFrame::fromScalar(Key* key, KVStore* kv, T value) {
    fromArray(key, kv, 1, &value);
}