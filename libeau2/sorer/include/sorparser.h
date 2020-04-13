#pragma once

#include "object.h"
#include "linereader.h"
#include "column.h"
#include "strslice.h"

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
