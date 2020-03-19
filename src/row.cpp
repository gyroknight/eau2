// lang::Cpp

#include "row.hpp"

Row::Row(Schema& scm) : __schema(scm), __idx(0), __data(scm.width()) {}

Row::~Row() {
    for (size_t ii = 0; ii < __data.size(); ii++) {
        if (__data[ii]) {
            switch (__schema.col_type(ii)) {
                case 'I':
                    delete static_cast<int*>(__data[ii]);
                    break;
                case 'B':
                    delete static_cast<bool*>(__data[ii]);
                    break;
                case 'D':
                    delete static_cast<double*>(__data[ii]);
                    break;
                    // we don't own our strings, no freeing needed
            }

            __data[ii] = nullptr;
        }
    }
}

/** Setters: set the given column with the given value. Setting a column
 * with a value of the wrong type is undefined. */
void Row::set(size_t col, int val) {
    if (__schema.col_type(col) != 'I') {
        std::cerr << "Column " << col << " is not integer" << std::endl;
        return;
    }

    __data[col] = new int(val);
}

void Row::set(size_t col, double val) {
    if (__schema.col_type(col) != 'D') {
        std::cerr << "Column " << col << " is not double" << std::endl;
        return;
    }

    __data[col] = new double(val);
}

void Row::set(size_t col, bool val) {
    if (__schema.col_type(col) != 'B') {
        std::cerr << "Column " << col << " is not bool" << std::endl;
        return;
    }

    __data[col] = new bool(val);
}

// String is external
void Row::set(size_t col, std::string& val) {
    if (__schema.col_type(col) != 'S') {
        std::cerr << "Column " << col << " is not string" << std::endl;
        return;
    }

    __data[col] = &val;
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
int Row::getInt(size_t col) { return *static_cast<int*>(__data[col]); }
bool Row::getBool(size_t col) { return *static_cast<bool*>(__data[col]); }
double Row::getDouble(size_t col) { return *static_cast<double*>(__data[col]); }
std::string& Row::getString(size_t col) {
    return *static_cast<std::string*>(__data[col]);
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