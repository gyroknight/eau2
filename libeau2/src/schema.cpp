#include "schema.hpp"

Schema::Schema(const Schema& from)
    : __rowNames(from.__rowNames),
      __colNames(from.__colNames),
      __colTypes(from.__colTypes) {}

Schema::Schema() {}

Schema::Schema(const char* types) {
    while (*types) {
        add_column(*types, "");
        types++;
    }
}

bool Schema::add_column(char typ, std::string name) {
    switch (typ) {
        case 'S':
        case 'B':
        case 'I':
        case 'D':
            __colTypes.push_back(typ);
            __colNames.push_back(name);
            return true;
        default:
            std::cerr << "Unknown column type '" << typ << "'" << std::endl;
            return false;
    }
}

/** Add a row with a name (possibly nullptr), name is external.  Names
 * are expectd to be unique, duplicates result in undefined behavior. */
void Schema::add_row(std::string name) { __rowNames.push_back(name); }

/** Return name of row at idx; nullptr indicates no name. An idx >=
 * width is undefined. */
std::string Schema::row_name(size_t idx) const { return __rowNames.at(idx); }

/** Return name of column at idx; nullptr indicates no name given.
 *  An idx >= width is undefined.*/
std::string Schema::col_name(size_t idx) const { return __colNames.at(idx); }

/** Return type of column at idx. An idx >= width is undefined. */
char Schema::col_type(size_t idx) const { return __colTypes.at(idx); }

/** Given a column name return its index, or -1. */
int Schema::col_idx(const char* name) const {
    for (size_t ii = 0; ii < __colNames.size(); ii++) {
        if (__colNames[ii] == name) {
            return ii;
        }
    }

    return -1;
}

/** Given a row name return its index, or -1. */
int Schema::row_idx(const char* name) const {
    for (size_t ii = 0; ii < __rowNames.size(); ii++) {
        if (__rowNames[ii] == name) {
            return ii;
        }
    }

    return -1;
}

/** The number of columns */
size_t Schema::width() const { return __colNames.size(); }

/** The number of rows */
size_t Schema::length() const { return __rowNames.size(); }