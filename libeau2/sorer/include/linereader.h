#pragma once

#include <cstdio>

#include "object.h"

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