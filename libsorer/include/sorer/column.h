/**
 * @file column.h
 * @author euhlmann (uhlmann.e@husky.neu.edu)
 * 
 * Used with permission and modifications by Michael Hebert (mike.s.hebert@gmail.com) and Vincent Zhao (zhao.v@northeastern.edu)
 */

#pragma once

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "sorer/object.h"

namespace ne {

/**
 * Enum for the different types of SoR columns this code supports.
 */
enum class ColumnType { STRING, INTEGER, FLOAT, BOOL, UNKNOWN };

/**
 * Base class containing common code for columns of all types
 */
class BaseColumn : public Object {
   public:
    /** Initial arraylist capacity */
    static const size_t DEFAULT_CAPACITY = 16;
    /** What kind of column this is */
    ColumnType _type;
    /** Array of booleans indicating whether entry is present or missing */
    bool* _entry_present;
    /** Length of entries in this column */
    size_t _length;
    /** Capacity of arrays before reallocation is necessary */
    size_t _capacity;

    /**
     * Constructs a BaseColumn type.
     * @param type The type of column
     * @param initial_capacity The initial capacity of the arraylists
     */
    BaseColumn(ColumnType type, size_t initial_capacity);

    /** Frees this BaseColumn */
    virtual ~BaseColumn();

    /**
     * Resizes the internal arrays if length has reached capacity.
     */
    virtual void _resize_if_necessary();

    /**
     * Resizes the entry present/missing bool array to a new size.
     * @param new_cap the requested size
     */
    virtual void _resize_entry_present(size_t new_cap);

    /**
     * Marks a given entry as present or not present
     * @param which The entry index
     * @param present Whether this entry is present (true) or missing (false)
     */
    virtual void _set_entry_present(size_t which, bool present);

    /**
     * Method to be implemented by subclasses that resizes their array of the
     * actual entries.
     * @param new_cap The requested size. Will always be greater than the
     * current capacity.
     */
    virtual void _resize_entries(size_t new_cap) = 0;
    /**
     * Appends a missing entry.
     */
    virtual void appendMissing() = 0;
    /**
     * Prints the entry with the given index to stdout. Internal unchecked
     * method for subclasses to implement.
     * @param which The entry index, guaranteed to represent a valid non-missing
     * entry
     */
    virtual void _printEntry(size_t which) = 0;

    /**
     * Prints the given entry to stdout, or <MISSING> if it's missing.
     * Terminates if which >= getLength()
     * @param which The entry to print
     */
    virtual void printEntry(size_t which);

    /**
     * Gets the type of this column.
     * @return The type
     */
    virtual ColumnType getType();

    /**
     * Returns the number of entries the column currently has.
     * @return The number of entries
     */
    virtual size_t getLength();

    /**
     * Checks if a given entry is present. Terminates if which >= getLength().
     * @param which The entry index
     */
    virtual bool isEntryPresent(size_t which);
};

/**
 * Subclass of BaseColumn holding strings.
 * Expects ownership of each appended string to be transferred to the
 * StringColumn. Frees contained strings in the destructor.
 */
class StringColumn : public BaseColumn {
   public:
    const char** _entries;
    StringColumn();

    virtual ~StringColumn();

    virtual void _resize_entries(size_t new_cap) override;

    /**
     * Appends a given string to the column.
     * Takes ownership of entry, which means
     * references to entry after calling this method may be invalid.
     * @param entry The string. Must not be null
     */
    virtual void append(const char* entry);

    virtual void appendMissing() override;

    virtual const char* getEntry(size_t which);

    virtual void _printEntry(size_t which) override;
};

/**
 * Subclass of BaseColumn holding integers.
 */
class IntegerColumn : public BaseColumn {
   public:
    int* _entries;
    IntegerColumn();
    virtual ~IntegerColumn();

    virtual void _resize_entries(size_t new_cap) override;

    virtual void append(int entry);


    virtual void appendMissing() override;


    virtual int getEntry(size_t which);

    virtual void _printEntry(size_t which) override;
};

/**
 * Subclass of BaseColumn holding floats.
 */
class FloatColumn : public BaseColumn {
   public:
    float* _entries;
    FloatColumn();
    virtual ~FloatColumn();

    virtual void _resize_entries(size_t new_cap) override;

    virtual void append(float entry);


    virtual void appendMissing() override;


    virtual float getEntry(size_t which);

    virtual void _printEntry(size_t which) override;
};

/**
 * Subclass of BaseColumn holding booleans.
 */
class BoolColumn : public BaseColumn {
   public:
    bool* _entries;
    BoolColumn();
    virtual ~BoolColumn();

    virtual void _resize_entries(size_t new_cap) override;

    virtual void append(bool entry);


    virtual void appendMissing() override;


    virtual bool getEntry(size_t which);

    virtual void _printEntry(size_t which) override;
};

/**
 * Represents a fixed-size set of columns of potentially different types.
 */
class ColumnSet : public Object {
   public:
    /** The array of columns */
    BaseColumn** _columns;
    /** The number of columns we have */
    size_t _length;
    /**
     * Creates a new ColumnSet that can hold the given number of columns.
     * Caller must also call initializeColumn for each column to fully
     * initialize this class.
     * @param num_columns The max number of columns that can be held
     */
    ColumnSet(size_t num_columns);
    
    /**
     * Destructor for ColumnSet
     */
    virtual ~ColumnSet();

    /**
     * Gets the number of columns that can be held in this ColumnSet.
     * @return The number of columns
     */
    virtual size_t getLength();

    /**
     * Initializes the given column to the given type. Can only be called
     * exactly once per index.
     * @param which The index for the column to initialize
     * @param type The type of column to create
     */
    virtual void initializeColumn(size_t which, ColumnType type);

    /**
     * Gets the column with the given index. initializeColumn must have been
     * called for this index first.
     * @param which The column index to retrieve
     * @return The column with the given index
     */
    virtual BaseColumn* getColumn(size_t which);
};

}  // namespace ne