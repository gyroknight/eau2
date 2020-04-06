// lang::Cpp
#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "chunk.hpp"

class ColumnInterface {
   public:
    virtual ~ColumnInterface(){};

    /** Returns the number of elements in the column. */
    virtual size_t size() const = 0;

    /** Returns the column as a string "1, 2, 3, 4" */
    virtual std::string str() const = 0;
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
    std::vector<std::shared_ptr<Chunk<T>>> _data;  // vector of chunks
    size_t _size;

   public:
    // construct a column
    Column();

    // Move constructor
    Column(Column<T>&& other) = delete;

    // Copy constructor - does not preserve internal structure, and does not
    // copy actual T elements.
    Column(const Column<T>& other) = delete;

    // Creates a column with the provided elements
    Column(std::initializer_list<T> ll);

    // Get a value at the given index
    T get(size_t idx) const;

    // Set value at idx. An out of bound idx is undefined
    void set(size_t idx, T val);

    // Adds a value to the end of the column
    void push_back(T val);

    /** Returns the number of elements in the column. */
    virtual size_t size() const override;

    /** Returns the column as a string "1, 2, 3, 4" */
    virtual std::string str() const override;
};

#include "column.tpp"