// lang::Cpp
/**
 * @file column.hpp
 * @author Vincent Zhao, Michael Hebert
 * @brief A container representing a column of data in a DataFrame.
 */
#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "chunk.hpp"

class Serializer;

/**
 * @brief A common interface representing a Column, allows for different Column
 * types to be stored within the same STL containers.
 */
class ColumnInterface {
   public:
    virtual ~ColumnInterface(){};

    /** Returns the number of elements in the column. */
    virtual size_t size() const = 0;

    /** Returns the column as a string "1, 2, 3, 4" */
    virtual std::string str() const = 0;

    //! Serializes the given Column
    virtual void serialize(Serializer& ss) const = 0;

    //! Checks if the Column contains serializable types
    virtual bool canSerialize() const = 0;
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
    size_t _size;  // number of items in the column

   public:
    // construct a column
    Column();

    // Columns cannot be moved from their intial instantiation
    Column(Column<T>&& other) = delete;

    // Columns cannot be copied
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
    size_t size() const override;

    /** Returns the column as a string "1, 2, 3, 4" */
    std::string str() const override;

    //! Serializes the given Column
    void serialize(Serializer& ss) const override;

    //! Checks if the Column contains serializable types
    bool canSerialize() const override;
};

#include "column.tpp"