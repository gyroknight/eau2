/**
 * @file schena.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "schema.hpp"

#include <iostream>
#include <memory>

Schema::Schema(const Schema& from)
    : _rowNames(from._rowNames),
      _colNames(from._colNames),
      _colTypes(from._colTypes),
      _local(from._local),
      _length(from._length) {}

Schema::Schema() : _local(true) {}

Schema::Schema(const char* types, bool local) : _local(local) {
    while (*types) {
        addCol(*types);
        types++;
    }
}

bool Schema::addCol(const char type, ExtString name) {
    switch (type) {
        case 'S':
        case 'B':
        case 'L':
        case 'I':
        case 'D':
        case 'F':
            _colTypes.push_back(type);
            _colNames.push_back(name);
            return true;
        default:
            std::cerr << "Unknown column type '" << type << "'" << std::endl;
            return false;
    }
}

/** Add a row with a name (possibly nullptr), name is external.  Names
 * are expectd to be unique, duplicates result in undefined behavior. */
void Schema::addRow(ExtString name) { _rowNames.push_back(name); }

/** Return name of row at idx; nullptr indicates no name. An idx >=
 * width is undefined. */
std::string Schema::rowName(size_t idx) const { return *_rowNames.at(idx); }

/** Return name of column at idx; nullptr indicates no name given.
 *  An idx >= width is undefined.*/
std::string Schema::colName(size_t idx) const { return *_colNames.at(idx); }

/** Return type of column at idx. An idx >= width is undefined. */
char Schema::colType(size_t idx) const { return _colTypes.at(idx); }

Serial::Type Schema::colSerialType(size_t idx) const {
    switch (_colTypes.at(idx)) {
        case 'S':
            return Serial::Type::String;
        case 'B':
            return Serial::Type::Bool;
        case 'L':
            return Serial::Type::I64;
        case 'I':
            return Serial::Type::I32;
        case 'D':
            return Serial::Type::Double;
        case 'F':
            return Serial::Type::Float;
        default:
            return Serial::Type::Unknown;
    }
}

/** Given a column name return its index, or -1. */
int Schema::colIdx(const char* name) const {
    for (size_t ii = 0; ii < _colNames.size(); ii++) {
        if (*_colNames[ii] == name) {
            return ii;
        }
    }

    return -1;
}

/** Given a row name return its index, or -1. */
int Schema::rowIdx(const char* name) const {
    for (size_t ii = 0; ii < _rowNames.size(); ii++) {
        if (*_rowNames[ii] == name) {
            return ii;
        }
    }

    return -1;
}

/** The number of columns */
size_t Schema::width() const { return _colNames.size(); }

/** The number of rows */
size_t Schema::length() const {
    if (_local) {
        return _rowNames.size();
    } else {
        return _length;
    }
}

bool Schema::isLocal() const { return _local; }