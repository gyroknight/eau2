// lang::Cpp
#pragma once

#include <string>
#include <thread>
#include <variant>
#include <vector>

#include "column.hpp"
#include "row.hpp"
#include "rower.hpp"
#include "schema.hpp"

#define COL_VARIANTS int, bool, float, std::string

using ExtString = std::shared_ptr<std::string>;

class KVStore;
class Key;

/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (int, std::string, bool, or float). A dataframe
 * has a schema that describes it.
 */
class DataFrame {
   private:
    Schema __schema;
    std::vector<std::shared_ptr<ColumnInterface>> __data;

    template <typename T>
    T getVal(size_t col, size_t row);

   public:
    /** Create a data frame from a schema and columns. All columns are created
     * empty. */
    DataFrame(const Schema& schema);

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
    void addCol(Column<T>& col, std::string name);

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
};

#include "dataframe.tpp"