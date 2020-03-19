// lang::Cpp
#pragma once

#include "chunk.hpp"

#include <array>
#include <string>
#include <vector>

class ColumnInterface {
   public:
    virtual ~ColumnInterface() = 0;
    virtual size_t size() const = 0;
};

/**
 * Column
 *
 * Represents one column of a data frame which holds values of a single type.
 * Columns are mutable, equality is pointer
 * equality.
 * */
template <typename T>
class Column : public ColumnInterface {
   private:
    std::vector<Chunk<T>>
        __data;  // vector of fixed-size arrays
                 // arrays_->get(0) is either size 1 or the exact
                 // size of the elements it was initialized with
                 // from arrays_->get(1)..arrays_->get(n), the
                 // arrays are sized according to 2^i, i.e.
                 // get(1)->size() = 2, get(2)->size() = 4, etc.

   public:
    Column();

    // Creates a column with the provided elements
    Column(std::initializer_list<T> ll);

    // Creates a value-accurate copy of the provided column
    Column(Column<T>& other);

    // Get a value at the given index
    T get(size_t idx);

    /** Set value at idx. And out of bound idx is undefined. */
    void set(size_t idx, T val);

    // Adds a value to the end of the column
    void push_back(T val);

    /** Returns the number of elements in the column. */
    size_t size() const { return __data.size(); }
};

#include "column.tpp"