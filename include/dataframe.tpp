// lang::Cpp
#pragma once

#include <exception>

/** Adds a column this dataframe, updates the schema, the new column
 * is external, and appears as the last column of the dataframe, the
 * name is optional and external. A nullptr colum is undefined. */
template <typename T>
void DataFrame::addCol(Column<T>& col, std::string name) {
    if (col->size() != _schema.length()) {
        std::cerr << "Column does not match schema length" << std::endl;
        return;
    }

    char type = col.type();

    if (_schema.add_column(type, name))
        _data.push_back(std::make_shared<Column<T>>(col));
    else
        throw std::invalid_argument();
}

template <typename T>
T DataFrame::getVal(size_t col, size_t row) {
    // cast column interface down to correct type
    auto dfcol = std::dynamic_pointer_cast<Column<T>>(_data.at(col));

    return dfcol->get(row);
}

/** Set the value at the given column and row to the given value.
 * If the column is not  of the right type or the indices are out of
 * bound, the result is undefined. */
template <typename T>
void DataFrame::set(size_t col, size_t row, T val) {
    auto dfCol = std::dynamic_pointer_cast<Column<T>>(_data.at(col));
    dfCol->set(row, val);
}