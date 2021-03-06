// lang::Cpp
/**
 * @file dataframe.hpp
 * @author Vincent Zhao, Michael Hebert
 * @brief A 2D columnar representation of data, can represent a whole set of
 * data or a subset. DataFrames can be hosted locally or pointers to remote
 * Dataframes.
 */
#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "commondefs.hpp"
#include "schema.hpp"

class ColumnInterface;
class KVStore;
class Key;
class Row;
class Rower;
namespace ne {
class ColumnSet;
}

/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (long, int, std::string, bool, double, or
 * float). A dataframe has a schema that describes it.
 */
class DataFrame {
   private:
    Schema _schema;  // specifies the layout of the dataframe
    std::vector<std::shared_ptr<ColumnInterface>>
        _data;    // columns of data in schema order
    bool _local;  // Defines whether the DataFrame's contents are local or not
                  // (DataFrame of DataFrames or local storage)
    KVStore* _kv = nullptr;  // Reference to KV store for remote DataFrames
    size_t
        _blkSize;  // Size of each keyed DataFrame block for remote DataFrames

    // Returns the value as the type specified, throws exception if invalid
    template <typename T>
    T getVal(size_t col, size_t row);

    // gives Payload access to private fields for serialization
    friend class Payload;

    template <typename T>
    static void fillColumn(ColPtr<T> col, T* arr, size_t size);

    // Used for filling remote DataFrame directories
    template <typename T>
    void _addRemoteCol(ColPtr<T> col);

   public:
    // Creates an empty DataFrame
    DataFrame();

    /** Create a data frame from a schema and columns. All columns are created
     * empty. */
    DataFrame(const Schema& schema, bool local = true);

    /**
     * Copy constructor - Create a data frame with the same columns as the given
     * df but with no rows or rownames
     */
    DataFrame(const DataFrame& df);

    /**
     * Move constructor - moves schema and data
     */
    DataFrame(DataFrame&& df);

    /**
     * Destroy the Data Frame object
     */
    ~DataFrame();

    /** Returns the dataframe's schema. Modifying the schema after a dataframe
     * has been created in undefined. */
    Schema& getSchema();

    /** Adds a column this dataframe, updates the schema, the new column
     * is external, and appears as the last column of the dataframe, the
     * name is optional and external. A nullptr colum is undefined. */
    template <typename T>
    void addCol(ColPtr<T> col, ExtString name = nullptr);

    /** Return the value at the given column and row. Accessing rows or
     *  columns out of bounds, or request the wrong type is undefined.*/
    int getInt(size_t col, size_t row);

    bool getBool(size_t col, size_t row);

    double getDouble(size_t col, size_t row);

    ExtString getString(size_t col, size_t row);

    /** Set the value at the given column and row to the given value.
     * If the column is not  of the right type or the indices are out of
     * bound, the result is undefined. */
    template <typename T>
    void set(size_t col, size_t row, T val);

    /** Set the fields of the given row object with values from the columns at
     * the given offset.  If the row is not form the same schema as the
     * dataframe, results are undefined.
     */
    void fillRow(size_t idx, Row& row);

    /** Add a row at the end of this dataframe. The row is expected to have
     *  the right schema and be filled with values, otherwise undedined.  */
    void add_row(Row& row);

    /** The number of rows in the dataframe. */
    size_t nrows();

    /** The number of columns in the dataframe.*/
    size_t ncols();

    /** Visit rows in order */
    void map(Rower& r);

    /** Create a new dataframe, constructed from rows for which the given Rower
     * returned true from its accept method. */
    DataFrame& filter(Rower& r);

    /** This method clones the Rower and executes the map in parallel. Join is
     * used at the end to merge the results. */
    void pmap(Rower& r);

    /**
     * @brief Creates a single-column DataFrame from the array provided and
     * stores it in the key-value store at the key provided.
     *
     * @tparam T
     * @param key
     * @param kv
     * @param size
     * @param array
     */
    template <typename T>
    static void fromArray(Key* key, KVStore* kv, size_t size, T* array);

    /**
     * @brief Creates a single-column, single-item DataFrame with the value
     * provided and stores it in the key-value store at the key provided.
     *
     * @tparam T
     * @param key
     * @param kv
     * @param value
     */
    template <typename T>
    static void fromScalar(Key* key, KVStore* kv, T value);

    /**
     * @brief Creates a database from 4500ne's parsers. There's some data
     * overhead that would be solved by refactoring our code or theirs to share
     * the same column types.
     *
     * @param key   the key in the store
     * @param kv    the store
     * @param set   the set of columns
     */
    static void fromColumnSet(Key* key, KVStore* kv, ne::ColumnSet* set);

    /**
     * @brief Creates a remote DataFrame out of a SoRer file and distributes
     * local blocks across KV store
     *
     * @param filename
     * @param key
     * @param kv
     * @return DFPtr
     */
    static DFPtr fromFile(const char* filename, const Key& key, KVStore* kv);

    /**
     * @brief Mostly for debugging.
     *
     */
    void print();
};

#include "dataframe.tpp"