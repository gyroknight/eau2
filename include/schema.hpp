// lang::CwC
#pragma once

#include <iostream>
#include <string>
#include <vector>

/*************************************************************************
 * Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns and number of rows, the type of each column,
 * optionally columns and rows can be named by strings.
 * The valid types are represented by the chars 'S', 'B', 'I' and 'F'.
 */
class Schema {
   private:
    std::vector<std::string*> __rowNames;  // names of rows
    std::vector<std::string*> __colNames;  // names of columns
    std::vector<char> __colTypes;          // types of columns

   public:
    /** Copying constructor */
    Schema(Schema& from);

    /** Create an empty schema **/
    Schema();

    /** Create a schema from a string of types. A string that contains
     * characters other than those identifying the four type results in
     * undefined behavior. The argument is external, a nullptr argument is
     * undefined. **/
    Schema(const char* types);

    /** Add a column of the given type and name (can be nullptr), name
     * is external. Names are expectd to be unique, duplicates result
     * in undefined behavior. */
    void add_column(char typ, std::string* name);

    /** Add a row with a name (possibly nullptr), name is external.  Names
     * are expectd to be unique, duplicates result in undefined behavior. */
    void add_row(std::string* name);

    /** Return name of row at idx; nullptr indicates no name. An idx >=
     * width is undefined. */
    std::string* row_name(size_t idx);

    /** Return name of column at idx; nullptr indicates no name given.
     *  An idx >= width is undefined.*/
    std::string* col_name(size_t idx);

    /** Return type of column at idx. An idx >= width is undefined. */
    char col_type(size_t idx);

    /** Given a column name return its index, or -1. */
    int col_idx(const char* name);

    /** Given a row name return its index, or -1. */
    int row_idx(const char* name);

    /** The number of columns */
    size_t width();

    /** The number of rows */
    size_t length();
};
