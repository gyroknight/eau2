/**
 * @file parser.h
 * @author euhlmann (uhlmann.e@husky.neu.edu)
 * 
 * Used with permission and modifications by Michael Hebert (mike.s.hebert@gmail.com) and Vincent Zhao (zhao.v@northeastern.edu)
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "sorer/column.h"

namespace ne {

/**
 * The maximum allowed length for string columns.
 */
constexpr const size_t MAX_STRING = 255;

/**
 * Represents a slice of a larger c-style string. Lightweight replacement for allocating/copying
 * regular c-style-strings.
 */
class StrSlice : public Object {
   public:
    size_t _start;
    size_t _end;
    const char* _str;
    /**
     * Creates a new StrSlice.
     * @param str The C-style-string to slice from. Must be valid during the lifetime of this slice
     * @param start The starting index
     * @param end The ending index
     */
    StrSlice(const char* str, size_t start, size_t end);

    /**
     * @return The length of this slice
     */
    virtual size_t getLength();
    /**
     * @return A non-null-terminated pointer to the chars in this slice.
     */
    virtual const char* getChars();
    /**
     * Gets the char at the given index.
     * @param which The index. Must be < getLength()
     * @return The char
     */
    virtual char getChar(size_t which);

    /**
     * Updates the bounds of this slice to exclude instances of the given char at the beginning or
     * end. May shorten the length of this slice.
     * @param c The char to remove from the beginning and end
     */
    virtual void trim(char c);
    
    /**
     * Allocates a new null terminated string and copies the contents of this slice to it.
     * @return The new string. Caller must free
     */
    virtual char* toCString();

    /**
     * Parses the contents of this slice as an int.
     * @return An int corresponding to the digits in this slice.
     */
    virtual int toInt();

    /**
     * Parses the contents of this slice as a float.
     * @return The float
     */
    virtual float toFloat();
};

/**
 * This class is capable of reading a given file line-by-line in a semi-efficient manner.
 * Additionally, it can be constrained to a given start and end position in the file, and will
 * discard the first and last (possibly partial) lines in this case.
 */
class LineReader : public Object {
   public:
    /** Temporary buffer for file contents */
    char _buf[4096];
    size_t _buf_length;
    /** Current position in buffer */
    size_t _pos;
    /** The file we're reading from */
    FILE* _file;
    /** Byte indices for start, end, and total file size */
    size_t _file_start;
    size_t _file_end;
    size_t _file_size;
    /** Heap-allocated current line being built by readLine() */
    char* _current_line;
    size_t _current_line_size;
    /** Total number of bytes read so far */
    size_t _read_size;

    /**
     * Constructs a new LineReader.
     * @param file The file to read from.
     * @param file_start the starting index (of bytes in the file)
     * @param file_end The ending index
     * @param file_size The total size of the file (as obtained by e.g. ftell)
     */
    LineReader(FILE* file, size_t file_start, size_t file_end, size_t file_size);

    /**
     * Destructor for LineReader
     */
    virtual ~LineReader();

    /**
     * Null-terminates the current line being built and returns it, setting the _current_line
     * member to null. Ownership of the line is transferred to the caller.
     * @return A new null terminated string for the completed line
     */
    virtual char* _get_current_line();

    /**
     * Appends a given section of the given string to the _current_line member.
     * @param src The string to use
     * @param start The starting index in the string to copy from
     * @param end The ending index in the string to copy up to
     */
    virtual void _append_current_line(const char* src, size_t start, size_t end);

    /**
     * The main method implemented by this type. Reads the next full line out of the file. If
     * starting from a nonzero offset or ending before the end of the file, the first and last
     * lines respectively are skipped.
     * @return The next line, or nullptr if we are out of lines
     */
    virtual char* readLine();

    /**
     * Resets this reader. Goes back to the start position and prepares to read lines from there
     * again.
     */
    virtual void reset();
};

/**
 * Enum representing what mode the parser is currently using for parsing.
 */
enum class ParserMode {
    /** We're trying to find the number of columns in the schema */
    DETECT_NUM_COLUMNS,
    /** We're guessing the column types */
    DETECT_SCHEMA,
    /** Parsing the whole file */
    PARSE_FILE
};

/**
 * Parses a given file into a ColumnSet with BaseColumns representing the sor data in the file.
 */
class SorParser : public Object {
   public:
    // Char constants for parsing
    static const size_t GUESS_SCHEMA_LINES = 500;
    static const char FIELD_BEGIN = '<';
    static const char FIELD_END = '>';
    static const char STRING_QUOTE = '"';
    static const char SPACE = ' ';
    static const char DOT = '.';
    static const char PLUS = '+';
    static const char MINUS = '-';

    /**
     * Checks if the given char could be part of an integer or float field.
     */
    static bool isNumeric(char c);

    /** LineReader we're using */
    LineReader* _reader;
    /** ColumnSet for data we will ultimately parse */
    ColumnSet* _columns;
    /** Array of guesses for the types of each column in the schema */
    ColumnType* _typeGuesses;
    /** The number of columns we have detected */
    size_t _num_columns;

    /**
     * Creates a new SorParser with the given parameters.
     * @param file The file to read from.
     * @param file_start the starting index (of bytes in the file)
     * @param file_end The ending index
     * @param file_size The total size of the file (as obtained by e.g. ftell)
     */
    SorParser(FILE* file, size_t file_start, size_t file_end, size_t file_size);

    /**
     * Destructor for SorParser
     */
    virtual ~SorParser();

    /**
     * Appends the next entry contained in the given StrSlice to the column at the given index,
     * using the type of the column.
     * @param slice The slice containing the data for this field
     * @param field_num The column index
     * @param columns The ColumnSet to add the data to
     */
    virtual void _appendField(StrSlice slice, size_t field_num, ColumnSet* columns);

    /**
     * Tries to guess or update the guess for the given column index given a field contained in the
     * given StrSlice.
     * @param slice The slice to use
     * @param field_num The column index
     */
    virtual void _guessFieldType(StrSlice slice, size_t field_num);

    /**
     * Finds and iterates over the deliminated fields in the given line string according to the
     * given parsing mode.
     * @param line The line to scan/parse
     * @param mode The mode to use
     * @param columns The data representation to update
     */
    virtual size_t _scanLine(const char* line, ParserMode mode, ColumnSet* columns);

    /**
     * Attempts to guess the schema based on the first 500 lines in the file.
     * Must be called first, before parseFile or getColumnSet. Can only be called once.
     */
    virtual void guessSchema();

    /**
     * Parses all the data in the file (between the start index and length).
     * guessSchema() must be called before this functions. Can only be called once.
     */
    virtual void parseFile();

    /**
     * Gets the in-memory representation for the sor data.
     * guessSchema() and parseFile() must be called before this function.
     */
    virtual ColumnSet* getColumnSet();
};

} // namespace ne
