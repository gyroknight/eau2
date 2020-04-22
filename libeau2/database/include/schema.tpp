/**
 * @file schena.tpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <iostream>

// For remote Schemas, the size of the Column doesn't matter since length is not
// tied to an actual Column
template <typename T>
inline bool Schema::addCol(const Column<T>& col, ExtString name) {
    const char colType = colToType(col);

    if (colType == 'U' || !col.size()) return false;

    if (_rowNames.empty()) _rowNames.resize(col.size());

    if (_rowNames.size() != col.size() && _local) {
        std::cerr << "Column does not match schema length" << std::endl;
        return false;
    }

    return addCol(colType, name);
}

// specializations
template <>
inline char Schema::colToType(const Column<long>& col) {
    return 'L';
}

template <>
inline char Schema::colToType(const Column<int>& col) {
    return 'I';
}

template <>
inline char Schema::colToType(const Column<double>& col) {
    return 'D';
}

template <>
inline char Schema::colToType(const Column<bool>& col) {
    return 'B';
}

template <>
inline char Schema::colToType(const Column<ExtString>& col) {
    return 'S';
}

template <typename T>
inline char Schema::colToType(const Column<T>& col) {
    return 'U';
}