#pragma once

#include <cassert>

#include "sorparser.h"
#include "column.h"
#include "strslice.h"

#define MAX_STRING 255

/**
 * Checks if the given char could be part of an integer or float field.
 */
bool SorParser::isNumeric(char c) {
    return c == MINUS || c == PLUS || c == DOT || (c >= '0' && c <= '9');
}

/**
 * Creates a new SorParser with the given parameters.
 * @param file The file to read from.
 * @param file_start the starting index (of bytes in the file)
 * @param file_end The ending index
 * @param file_size The total size of the file (as obtained by e.g. ftell)
 */
SorParser::SorParser(FILE* file, size_t file_start, size_t file_end, size_t file_size) : Object() {
    _reader = new LineReader(file, file_start, file_end, file_size);
    _columns = nullptr;
    _typeGuesses = nullptr;
    _num_columns = 0;
}

/**
 * Destructor for SorParser
 */
SorParser::~SorParser() {
    delete _reader;
    if (_columns != nullptr) {
        delete _columns;
    }
    if (_typeGuesses != nullptr) {
        delete[] _typeGuesses;
    }
}

/**
 * Appends the next entry contained in the given StrSlice to the column at the given index,
 * using the type of the column.
 * @param slice The slice containing the data for this field
 * @param field_num The column index
 * @param columns The ColumnSet to add the data to
 */
void SorParser::_appendField(StrSlice slice, size_t field_num, ColumnSet* columns) {
    slice.trim(SPACE);

    BaseColumn* column = columns->getColumn(field_num);

    if (slice.getLength() == 0) {
        column->appendMissing();
        return;
    }

    switch (column->getType()) {
        case ColumnType::STRING:
            slice.trim(STRING_QUOTE);
            assert(slice.getLength() <= MAX_STRING);
            dynamic_cast<StringColumn*>(column)->append(slice.toCString());
            break;
        case ColumnType::INTEGER:
            dynamic_cast<IntegerColumn*>(column)->append(slice.toInt());
            break;
        case ColumnType::FLOAT:
            dynamic_cast<FloatColumn*>(column)->append(slice.toFloat());
            break;
        case ColumnType::BOOL:
            dynamic_cast<BoolColumn*>(column)->append(slice.toInt() == 1);
            break;
        default:
            assert(false);
    }
}

/**
 * Tries to guess or update the guess for the given column index given a field contained in the
 * given StrSlice.
 * @param slice The slice to use
 * @param field_num The column index
 */
void SorParser::_guessFieldType(StrSlice slice, size_t field_num) {
    slice.trim(SPACE);
    if (slice.getLength() == 0) {
        return;
    }

    // Check if the slice consists of only numeric chars
    // and specifically whether it has a . (indicating float)
    bool is_numeric = false;
    bool has_dot = false;
    for (size_t i = 0; i < slice.getLength(); i++) {
        char c = slice.getChar(i);
        if (isNumeric(c)) {
            is_numeric = true;
            if (c == DOT) {
                has_dot = true;
            }
        }
    }
    // If the guess is already string, we can't change that because that means we have
    // seen a non-numeric entry already
    if (_typeGuesses[field_num] != ColumnType::STRING) {
        if (is_numeric && !has_dot) {
            // If it's an integer (not float), check if it's 0 or 1, which would indicate a
            // bool column
            int val = slice.toInt();
            if ((val == 0 || val == 1) && _typeGuesses[field_num] != ColumnType::INTEGER &&
                _typeGuesses[field_num] != ColumnType::FLOAT) {
                // Only keep the bool column guess if we haven't already guessed integer or
                // float (because that means we have seen non-bool values)
                _typeGuesses[field_num] = ColumnType::BOOL;
            } else if (_typeGuesses[field_num] != ColumnType::FLOAT) {
                // Use integer guess only if we didn't already guess float (which could not be
                // parsed as integer)
                _typeGuesses[field_num] = ColumnType::INTEGER;
            }
        } else if (is_numeric && has_dot) {
            // If there's a dot, this must be a float column
            _typeGuesses[field_num] = ColumnType::FLOAT;
        } else {
            // If there are non-numeric chars then this must be a string column
            _typeGuesses[field_num] = ColumnType::STRING;
        }
    }
}

/**
 * Finds and iterates over the deliminated fields in the given line string according to the
 * given parsing mode.
 * @param line The line to scan/parse
 * @param mode The mode to use
 * @param columns The data representation to update
 */
size_t SorParser::_scanLine(const char* line, ParserMode mode, ColumnSet* columns) {
    size_t num_fields = 0;
    size_t this_field_start = 0;
    bool in_field = false;
    bool in_string = false;

    // Iterate over the line, create slices for each detected field, and call either
    // _guessFieldType for ParserMode::DETECT_SCHEMA or _appendField for ParserMode::PARSE_FILE
    // for ParserMode::DETECT_NUM_COLUMNS we simply return the number of fields we saw
    for (size_t i = 0; i < strlen(line); i++) {
        char c = line[i];
        if (!in_field) {
            if (c == FIELD_BEGIN) {
                in_field = true;
                this_field_start = i;
            }
        } else {
            if (c == STRING_QUOTE) {
                // Allow > inside quoted strings
                in_string = !in_string;
            } else if (c == FIELD_END && !in_string) {
                if (mode == ParserMode::DETECT_SCHEMA) {
                    _guessFieldType(StrSlice(line, this_field_start + 1, i), num_fields);
                } else if (mode == ParserMode::PARSE_FILE) {
                    _appendField(StrSlice(line, this_field_start + 1, i), num_fields, columns);
                }
                in_field = false;
                num_fields++;
            }
        }
    }

    return num_fields;
}

/**
 * Attempts to guess the schema based on the first 500 lines in the file.
 * Must be called first, before parseFile or getColumnSet. Can only be called once.
 */
void SorParser::guessSchema() {
    assert(_columns == nullptr);
    assert(_typeGuesses == nullptr);
    // Detect the row with the most fields in the first 500 lines
    size_t max_columns = 0;
    for (size_t i = 0; i < GUESS_SCHEMA_LINES; i++) {
        char* next_line = _reader->readLine();
        if (next_line == nullptr) {
            break;
        }
        size_t num_columns = _scanLine(next_line, ParserMode::DETECT_NUM_COLUMNS, nullptr);
        if (num_columns > max_columns) {
            max_columns = num_columns;
        }
        delete[] next_line;
    }
    assert(max_columns != 0);

    // Guess the type for each column
    _reader->reset();
    _columns = new ColumnSet(max_columns);
    _typeGuesses = new ColumnType[max_columns];
    _num_columns = max_columns;
    for (size_t i = 0; i < _num_columns; i++) {
        _typeGuesses[i] = ColumnType::UNKNOWN;
    }

    for (size_t i = 0; i < GUESS_SCHEMA_LINES; i++) {
        char* next_line = _reader->readLine();
        if (next_line == nullptr) {
            break;
        }
        _scanLine(next_line, ParserMode::DETECT_SCHEMA, nullptr);
        delete[] next_line;
    }

    for (size_t i = 0; i < _num_columns; i++) {
        if (_typeGuesses[i] == ColumnType::UNKNOWN) {
            // Assume bool for anything we still don't have a guess for as per spec
            _typeGuesses[i] = ColumnType::BOOL;
        }
        _columns->initializeColumn(i, _typeGuesses[i]);
    }
}

/**
 * Parses all the data in the file (between the start index and length).
 * guessSchema() must be called before this functions. Can only be called once.
 */
void SorParser::parseFile() {
    assert(_columns != nullptr);

    _reader->reset();

    char* line;
    while (true) {
        line = _reader->readLine();
        if (line == nullptr) {
            break;
        }
        size_t scanned_fields = _scanLine(line, ParserMode::PARSE_FILE, _columns);
        for (size_t i = scanned_fields; i < _num_columns; i++) {
            _columns->getColumn(i)->appendMissing();
        }
        delete[] line;
    }
}

/**
 * Gets the in-memory representation for the sor data.
 * guessSchema() and parseFile() must be called before this function.
 */
ColumnSet* SorParser::getColumnSet() {
    assert(_columns != nullptr);

    return _columns;
}
