/**
 * @file row.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <cstddef>
#include <variant>
#include <vector>

#include "commondefs.hpp"

class Fielder;
class Schema;

/*************************************************************************
 * Row::
 *
 * This class represents a single row of data constructed according to a
 * dataframe's schema. The purpose of this class is to make it easier to add
 * read/write complete rows. Internally a dataframe hold data in columns.
 * Rows have pointer equality.
 */
class Row {
   private:
    Schema& _schema;
    size_t _idx;

    // TODO this is pretty ugly
    std::vector<std::variant<int, double, bool, ExtString>> _data;

   public:
    /** Build a row following a schema. */
    Row(Schema& scm);

    /** Setters: set the given column with the given value. Setting a column
     * with a value of the wrong type is undefined. */
    void set(size_t col, int val);

    void set(size_t col, double val);

    void set(size_t col, bool val);

    // String is external
    void set(size_t col, ExtString val);

    /** Set/get the index of this row (ie. its position in the dataframe. This
     * is only used for informational purposes, unused otherwise */
    void setIdx(size_t idx);

    size_t getIdx();

    /** Getters: get the value at the given column. If the column is not
     * of the requested type, the result is undefined. */
    int getInt(size_t col);
    bool getBool(size_t col);
    double getDouble(size_t col);
    ExtString getString(size_t col);

    /** Number of fields in the row. */
    size_t width();

    /** Type of the field at the given position. An idx >= width is  undefined.
     */
    char col_type(size_t idx);

    /** Given a Fielder, visit every field of this row. The first argument is
     * index of the row in the dataframe.
     * Calling this method before the row's fields have been set is undefined.
     */
    void visit(size_t idx, Fielder& f);
};
