/**
 * @file schema.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "commondefs.hpp"

template <typename T>
class Column;

/*************************************************************************
 * Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns and number of rows, the type of each column,
 * optionally columns and rows can be named by strings.
 * The valid types are represented by the chars 'S', 'B', 'L', 'I', 'D', and
 * 'F'.
 */
class Schema {
   private:
    std::vector<ExtString> _rowNames;  // names of rows
    std::vector<ExtString> _colNames;  // names of columns
    std::vector<char> _colTypes;       // types of columns

   public:
    /** Copying constructor */
    Schema(const Schema& from);

    /** Create an empty schema **/
    Schema();

    /** Create a schema from a string of types. A string that contains
     * characters other than those identifying the four type results in
     * undefined behavior. The argument is external, a nullptr argument is
     * undefined. **/
    Schema(const char* types);

    /** Add a column of the given type and name (can be nullptr), name
     * is external. Names are expectd to be unique, duplicates result
     * in undefined behavior.
     * Returns whether the column was added to the schema succesfully (whether
     * the type is supported).
     */
    template <typename T>
    bool addCol(const Column<T>& col, ExtString name = nullptr);

    bool addCol(char type, ExtString name = nullptr);

    /** Add a row with a name (possibly nullptr), name is external.  Names
     * are expectd to be unique, duplicates result in undefined behavior. */
    void addRow(ExtString name);

    /** Return name of row at idx; nullptr indicates no name. An idx >=
     * width is undefined. */
    std::string row_name(size_t idx) const;

    /** Return name of column at idx; nullptr indicates no name given.
     *  An idx >= width is undefined.*/
    std::string col_name(size_t idx) const;

    /** Return type of column at idx. An idx >= width is undefined. */
    char col_type(size_t idx) const;

    /** Given a column name return its index, or -1. */
    int col_idx(const char* name) const;

    /** Given a row name return its index, or -1. */
    int row_idx(const char* name) const;

    /** The number of columns */
    size_t width() const;

    /** The number of rows */
    size_t length() const;

    template <typename T>
    static char colToType(const Column<T>& col);
};

#include "schema.tpp"
