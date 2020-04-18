/**
 * @file row.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "row.hpp"

#include <iostream>

#include "fielder.hpp"
#include "schema.hpp"

Row::Row(Schema& scm) : _schema(scm), _idx(0), _data(scm.width()) {}

/** Setters: set the given column with the given value. Setting a column
 * with a value of the wrong type is undefined. */
void Row::set(size_t col, int val) { _data[col] = val; }

void Row::set(size_t col, double val) { _data[col] = val; }

void Row::set(size_t col, bool val) { _data[col] = val; }

// String is external
void Row::set(size_t col, ExtString val) { _data[col] = val; }

/** Set/get the index of this row (ie. its position in the dataframe. This
 * is only used for informational purposes, unused otherwise */
void Row::setIdx(size_t idx) {
    if (idx < _schema.length()) {
        _idx = idx;
        return;
    }

    std::cerr << "Invalid row index " << idx << std::endl;
}

size_t Row::getIdx() { return _idx; }

/** Getters: get the value at the given column. If the column is not
 * of the requested type, the result is undefined. */
int Row::getInt(size_t col) { return std::get<int>(_data.at(col)); }
bool Row::getBool(size_t col) { return std::get<bool>(_data.at(col)); }
double Row::getDouble(size_t col) { return std::get<double>(_data.at(col)); }
ExtString Row::getString(size_t col) {
    return std::get<ExtString>(_data.at(col));
}

/** Number of fields in the row. */
size_t Row::width() { return _data.size(); }

/** Type of the field at the given position. An idx >= width is  undefined.
 */
char Row::col_type(size_t idx) { return _schema.col_type(idx); }

/** Given a Fielder, visit every field of this row. The first argument is
 * index of the row in the dataframe.
 * Calling this method before the row's fields have been set is undefined.
 */
void Row::visit(size_t idx, Fielder& f) {
    if (idx != _idx) {
        std::cerr << "Row index mismatch" << std::endl;
        return;
    }

    f.start(idx);

    for (size_t ii = 0; ii < _data.size(); ii++) {
        // todo use std::visit maybe
        char type = col_type(ii);

        switch (type) {
            case 'I':
                f.accept(getInt(ii));
                break;
            case 'B':
                f.accept(getBool(ii));
                break;
            case 'D':
                f.accept(getDouble(ii));
                break;
            case 'S':
                f.accept(getString(ii));
                break;
            default:
                std::cerr << "Unsupported type '" << type << "'" << std::endl;
                break;
        }
    }

    f.done();
}