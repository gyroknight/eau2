// lang::Cpp

#include "row.hpp"

Row::Row(Schema& scm) : __schema(scm), __idx(0), __data(scm.width()) {}

/** Setters: set the given column with the given value. Setting a column
 * with a value of the wrong type is undefined. */
void Row::set(size_t col, int val) {
    if (auto pval = std::get_if<int>(&__data[col])) {
        *pval = val;
    } else {
        std::cerr << "Column " << col << " is not integer" << std::endl;
    }
}

void Row::set(size_t col, double val) {
    if (auto pval = std::get_if<double>(&__data[col])) {
        *pval = val;
    } else {
        std::cerr << "Column " << col << " is not double" << std::endl;
    }
}

void Row::set(size_t col, bool val) {
    if (auto pval = std::get_if<bool>(&__data[col])) {
        *pval = val;
    } else {
        std::cerr << "Column " << col << " is not bool" << std::endl;
    }
}

// String is external
void Row::set(size_t col, ExtString val) {
    if (auto pval = std::get_if<ExtString>(&__data[col])) {
        *pval = val;
    } else {
        std::cerr << "Column " << col << " is not ExtString" << std::endl;
    }
}

/** Set/get the index of this row (ie. its position in the dataframe. This
 * is only used for informational purposes, unused otherwise */
void Row::setIdx(size_t idx) {
    if (idx < __schema.length()) {
        __idx = idx;
        return;
    }

    std::cerr << "Invalid row index " << idx << std::endl;
}

size_t Row::getIdx() { return __idx; }

/** Getters: get the value at the given column. If the column is not
 * of the requested type, the result is undefined. */
std::shared_ptr<int> Row::getInt(size_t col) {
    return std::get<std::shared_ptr<int>>(__data[col]);
}
std::shared_ptr<bool> Row::getBool(size_t col) {
    return std::get<std::shared_ptr<bool>>(__data[col]);
}
std::shared_ptr<double> Row::getDouble(size_t col) {
    return std::get<std::shared_ptr<double>>(__data[col]);
}
ExtString Row::getString(size_t col) {
    return std::get<ExtString>(__data[col]);
}

/** Number of fields in the row. */
size_t Row::width() { return __data.size(); }

/** Type of the field at the given position. An idx >= width is  undefined.
 */
char Row::col_type(size_t idx) { return __schema.col_type(idx); }

/** Given a Fielder, visit every field of this row. The first argument is
 * index of the row in the dataframe.
 * Calling this method before the row's fields have been set is undefined.
 */
void Row::visit(size_t idx, Fielder& f) {
    if (idx != __idx) {
        std::cerr << "Row index mismatch" << std::endl;
        return;
    }

    f.start(idx);

    for (size_t ii = 0; ii < __data.size(); ii++) {
        // todo use std::visit maybe
        char type = col_type(ii);

        switch (type) {
            case 'I':
                f.accept(getInt(ii));
                break;
            case 'B':
                f.accept(getBool(ii));
                break;
            case 'F':
                f.accept(getDouble(ii));
                break;
            case 'S':
                f.accept(getString(ii));
                break;
        }
    }

    f.done();
}