/**
 * @file schena.tpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <iostream>

template <typename T>
inline bool Schema::addCol(const Column<T>& col, ExtString name) {
    const char colType = colToType(col);

    if (colType == 'U' || !col.size()) return false;

    if (_rowNames.empty()) _rowNames.resize(col.size());

    if (_rowNames.size() != col.size()) {
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
inline char Schema::colToType(const Column<std::string>& col) {
    return 'S';
}

template <typename T>
inline char Schema::colToType(const Column<T>& col) {
    return 'U';
}