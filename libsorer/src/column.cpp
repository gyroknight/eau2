/**
 * @file column.cpp
 * @author euhlmann (uhlmann.e@husky.neu.edu)
 * 
 * Used with permission and modifications by Michael Hebert (mike.s.hebert@gmail.com) and Vincent Zhao (zhao.v@northeastern.edu)
 */

#include "sorer/object.h"
#include "sorer/column.h"

namespace ne {

/**
 * Converts the given ColumnType to a string.
 * @param type the column type
 * @return A string representing this column type. Do not free or modify this string.
 */
const char* columnTypeToString(ColumnType type) {
    switch (type) {
        case ColumnType::STRING:
            return "STRING";
        case ColumnType::INTEGER:
            return "INTEGER";
        case ColumnType::FLOAT:
            return "FLOAT";
        case ColumnType::BOOL:
            return "BOOL";
        default:
            return "UNKNOWN";
    }
}

/**
 * Constructs a BaseColumn type.
 * @param type The type of column
 * @param initial_capacity The initial capacity of the arraylists
 */
BaseColumn::BaseColumn(ColumnType type, size_t initial_capacity) : Object() {
    _type = type;
    _capacity = initial_capacity;
    _length = 0;
    _entry_present = new bool[_capacity];
}

/** Frees this BaseColumn */
BaseColumn::~BaseColumn() { delete[] _entry_present; }

/**
 * Resizes the internal arrays if length has reached capacity.
 */
void BaseColumn::_resize_if_necessary() {
    if (_length == _capacity) {
        size_t new_cap = _capacity * 2;
        _resize_entries(new_cap);
        _capacity = new_cap;
    }
}

/**
 * Resizes the entry present/missing bool array to a new size.
 * @param new_cap the requested size
 */
void BaseColumn::_resize_entry_present(size_t new_cap) {
    bool* new_entry_present = new bool[new_cap];
    memcpy(new_entry_present, _entry_present, _capacity * sizeof(bool));
    delete[] _entry_present;
    _entry_present = new_entry_present;
}

/**
 * Marks a given entry as present or not present
 * @param which The entry index
 * @param present Whether this entry is present (true) or missing (false)
 */
void BaseColumn::_set_entry_present(size_t which, bool present) { _entry_present[which] = present; }

/**
 * Prints the given entry to stdout, or <MISSING> if it's missing.
 * Terminates if which >= getLength()
 * @param which The entry to print
 */
void BaseColumn::printEntry(size_t which) {
    assert(which < _length);
    if (!isEntryPresent(which)) {
        printf("<MISSING>\n");
    } else {
        _printEntry(which);
    }
}

/**
 * Gets the type of this column.
 * @return The type
 */
ColumnType BaseColumn::getType() { return _type; }

/**
 * Returns the number of entries the column currently has.
 * @return The number of entries
 */
size_t BaseColumn::getLength() { return _length; }

/**
 * Checks if a given entry is present. Terminates if which >= getLength().
 * @param which The entry index
 */
bool BaseColumn::isEntryPresent(size_t which) {
    assert(which < _length);
    return _entry_present[which];
}

StringColumn::StringColumn() : BaseColumn(ColumnType::STRING, DEFAULT_CAPACITY) {
    _entries = new const char*[_capacity];
}

StringColumn::~StringColumn() {
    for (size_t i = 0; i < _length; i++) {
        if (_entries[i] != nullptr) {
            delete[] _entries[i];
        }
    }
    delete[] _entries;
}

void StringColumn::_resize_entries(size_t new_cap) {
    _resize_entry_present(new_cap);
    const char** new_entries = new const char*[new_cap];
    memcpy(new_entries, _entries, _capacity * sizeof(char*));
    delete[] _entries;
    _entries = new_entries;
}

/**
 * Appends a given string to the column.
 * Takes ownership of entry, which means
 * references to entry after calling this method may be invalid.
 * @param entry The string. Must not be null
 */
void StringColumn::append(const char* entry) {
    _resize_if_necessary();
    _set_entry_present(_length, true);

    _entries[_length] = entry;

    _length += 1;
}

void StringColumn::appendMissing() {
    _resize_if_necessary();
    _set_entry_present(_length, false);

    _entries[_length] = nullptr;
    _length += 1;
}

const char* StringColumn::getEntry(size_t which) {
    assert(which < _length);
    assert(isEntryPresent(which));
    return _entries[which];
}

void StringColumn::_printEntry(size_t which) { printf("\"%s\"\n", _entries[which]); }

IntegerColumn::IntegerColumn() : BaseColumn(ColumnType::INTEGER, DEFAULT_CAPACITY) {
    _entries = new int[_capacity];
}
IntegerColumn::~IntegerColumn() { delete[] _entries; }

void IntegerColumn::_resize_entries(size_t new_cap) {
    _resize_entry_present(new_cap);
    int* new_entries = new int[new_cap];
    memcpy(new_entries, _entries, _capacity * sizeof(int));
    delete[] _entries;
    _entries = new_entries;
}

void IntegerColumn::append(int entry) {
    _resize_if_necessary();
    _set_entry_present(_length, true);

    _entries[_length] = entry;
    _length += 1;
}

void IntegerColumn::appendMissing() {
    _resize_if_necessary();
    _set_entry_present(_length, false);

    _entries[_length] = 0;
    _length += 1;
}

int IntegerColumn::getEntry(size_t which) {
    assert(which < _length);
    assert(isEntryPresent(which));
    return _entries[which];
}

void IntegerColumn::_printEntry(size_t which) { printf("%d\n", _entries[which]); }

FloatColumn::FloatColumn() : BaseColumn(ColumnType::FLOAT, DEFAULT_CAPACITY) {
    _entries = new float[_capacity];
}
FloatColumn::~FloatColumn() { delete[] _entries; }

void FloatColumn::_resize_entries(size_t new_cap) {
    _resize_entry_present(new_cap);
    float* new_entries = new float[new_cap];
    memcpy(new_entries, _entries, _capacity * sizeof(float));
    delete[] _entries;
    _entries = new_entries;
}

void FloatColumn::append(float entry) {
    _resize_if_necessary();
    _set_entry_present(_length, true);

    _entries[_length] = entry;
    _length += 1;
}

void FloatColumn::appendMissing() {
    _resize_if_necessary();
    _set_entry_present(_length, false);

    _entries[_length] = 0;
    _length += 1;
}

float FloatColumn::getEntry(size_t which) {
    assert(which < _length);
    assert(isEntryPresent(which));
    return _entries[which];
}

void FloatColumn::_printEntry(size_t which) { printf("%e\n", _entries[which]); }

BoolColumn::BoolColumn() : BaseColumn(ColumnType::BOOL, DEFAULT_CAPACITY) {
    _entries = new bool[_capacity];
}
BoolColumn::~BoolColumn() { delete[] _entries; }

void BoolColumn::_resize_entries(size_t new_cap) {
    _resize_entry_present(new_cap);
    bool* new_entries = new bool[new_cap];
    memcpy(new_entries, _entries, _capacity * sizeof(bool));
    delete[] _entries;
    _entries = new_entries;
}

void BoolColumn::append(bool entry) {
    _resize_if_necessary();
    _set_entry_present(_length, true);

    _entries[_length] = entry;
    _length += 1;
}

void BoolColumn::appendMissing() {
    _resize_if_necessary();
    _set_entry_present(_length, false);

    _entries[_length] = 0;
    _length += 1;
}

bool BoolColumn::getEntry(size_t which) {
    assert(which < _length);
    assert(isEntryPresent(which));
    return _entries[which];
}

void BoolColumn::_printEntry(size_t which) { printf("%d\n", _entries[which]); }

/**
 * Creates a new ColumnSet that can hold the given number of columns.
 * Caller must also call initializeColumn for each column to fully initialize this class.
 * @param num_columns The max number of columns that can be held
 */
ColumnSet::ColumnSet(size_t num_columns) : Object() {
    _columns = new BaseColumn*[num_columns];
    _length = num_columns;
    for (size_t i = 0; i < num_columns; i++) {
        _columns[i] = nullptr;
    }
}
/**
 * Destructor for ColumnSet
 */
ColumnSet::~ColumnSet() {
    for (size_t i = 0; i < _length; i++) {
        if (_columns[i] != nullptr) {
            delete _columns[i];
        }
    }
    delete[] _columns;
}

/**
 * Gets the number of columns that can be held in this ColumnSet.
 * @return The number of columns
 */
size_t ColumnSet::getLength() { return _length; }



/**
 * Creates the right subclass of BaseColumn based on the given type.
 * @param type The type of column to create
 * @return The newly created column. Caller must free.
 */
BaseColumn* makeColumnFromType(ColumnType type) {
    switch (type) {
        case ColumnType::STRING:
            return new StringColumn();
        case ColumnType::INTEGER:
            return new IntegerColumn();
        case ColumnType::FLOAT:
            return new FloatColumn();
        case ColumnType::BOOL:
            return new BoolColumn();
        default:
            assert(false);
    }
}

/**
 * Initializes the given column to the given type. Can only be called exactly once per index.
 * @param which The index for the column to initialize
 * @param type The type of column to create
 */
void ColumnSet::initializeColumn(size_t which, ColumnType type) {
    assert(which < _length);
    assert(_columns[which] == nullptr);
    BaseColumn* col = makeColumnFromType(type);
    _columns[which] = col;
}

/**
 * Gets the column with the given index. initializeColumn must have been called for this index
 * first.
 * @param which The column index to retrieve
 * @return The column with the given index
 */
BaseColumn* ColumnSet::getColumn(size_t which) {
    assert(which < _length);
    assert(_columns[which] != nullptr);
    return _columns[which];
}

} //namespace ne