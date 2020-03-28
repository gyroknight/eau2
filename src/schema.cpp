#include "schema.hpp"

Schema::Schema(const Schema& from)
    : _rowNames(from._rowNames),
      _colNames(from._colNames),
      _colTypes(from._colTypes) {}

Schema::Schema() {}

Schema::Schema(const char* types) {
    while (*types) {
        add_column(*types, nullptr);
        types++;
    }
}

void Schema::add_column(char typ, std::string name) {
    switch (typ) {
        case 'S':
        case 'B':
        case 'I':
        case 'D':
            _colTypes.push_back(typ);
            break;
        default:
            std::cerr << "Unknown column type" << std::endl;
    }
}

/** Add a row with a name (possibly nullptr), name is external.  Names
 * are expectd to be unique, duplicates result in undefined behavior. */
void Schema::add_row(std::string name) { _rowNames.push_back(name); }

/** Return name of row at idx; nullptr indicates no name. An idx >=
 * width is undefined. */
std::string Schema::row_name(size_t idx) { return _rowNames.at(idx); }

/** Return name of column at idx; nullptr indicates no name given.
 *  An idx >= width is undefined.*/
std::string Schema::col_name(size_t idx) { return _colNames.at(idx); }

/** Return type of column at idx. An idx >= width is undefined. */
char Schema::col_type(size_t idx) { return _colTypes.at(idx); }

/** Given a column name return its index, or -1. */
int Schema::col_idx(const char* name) {
    for (size_t ii = 0; ii < _colNames.size(); ii++) {
        if (*_colNames[ii] == name) {
            return ii;
        }
    }

    return -1;
}

/** Given a row name return its index, or -1. */
int Schema::row_idx(const char* name) {
    for (size_t ii = 0; ii < _rowNames.size(); ii++) {
        if (*_rowNames[ii] == name) {
            return ii;
        }
    }

    return -1;
}

/** The number of columns */
size_t Schema::width() { return _colNames.size(); }

/** The number of rows */
size_t Schema::length() { return _rowNames.size(); }