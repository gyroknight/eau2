// lang::Cpp
#pragma once

#include <exception>

/** Adds a column this dataframe, updates the schema, the new column
 * is external, and appears as the last column of the dataframe, the
 * name is optional and external. A nullptr colum is undefined. */
template <typename T>
void DataFrame::addCol(Column<T>& col, std::string* name) {
    if (col->size() != __schema.length()) {
        std::cerr << "Column does not match schema length" << std::endl;
        return;
    }

    char type = Schema::colToType<T>(col);

    if (__schema.add_column(type, name))
        __data.push_back(col);
    else
        throw std::invalid_exception;
}

template <typename T>
inline T DataFrame::getVal(size_t col, size_t row) {
    Column<T>& dfCol = dynamic_cast<Column<T>&>(*__data.at(col));

    return dfCol.get(row);
}

/** Set the value at the given column and row to the given value.
 * If the column is not  of the right type or the indices are out of
 * bound, the result is undefined. */
template <typename T>
inline void set(size_t col, size_t row, T val) {
    Column<T>& dfCol = dynamic_cast<Column<T>&>(*__data.get(col));
    dfCol.set(row, val);
}