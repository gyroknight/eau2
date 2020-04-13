#include <cstring>

#include "linereader.h"

/**
 * Constructs a new LineReader.
 * @param file The file to read from.
 * @param file_start the starting index (of bytes in the file)
 * @param file_end The ending index
 * @param file_size The total size of the file (as obtained by e.g. ftell)
 */
LineReader::LineReader(FILE* file, size_t file_start, size_t file_end, size_t file_size) : Object() {
    _file = file;
    _file_start = file_start;
    _file_end = file_end;
    _file_size = file_size;
    fseek(file, file_start, SEEK_SET);
    _current_line = nullptr;
    _current_line_size = 0;
    _pos = 0;
    _buf_length = 0;
    _read_size = 0;
}

/**
 * Destructor for LineReader
 */
LineReader::~LineReader() {
    if (_current_line != nullptr) {
        delete[] _current_line;
    }
}

/**
 * Null-terminates the current line being built and returns it, setting the _current_line
 * member to null. Ownership of the line is transferred to the caller.
 * @return A new null terminated string for the completed line
 */
char* LineReader::_get_current_line() {
    if (_current_line == nullptr) {
        return nullptr;
    }

    char* line = _current_line;
    line[_current_line_size] = '\0';
    _current_line_size = 0;
    _current_line = nullptr;
    return line;
}

/**
 * Appends a given section of the given string to the _current_line member.
 * @param src The string to use
 * @param start The starting index in the string to copy from
 * @param end The ending index in the string to copy up to
 */
void LineReader::_append_current_line(const char* src, size_t start, size_t end) {
    char* new_line;
    size_t copy_pos;
    if (_current_line == nullptr) {
        // Allocated a new heap buffer for the line
        new_line = new char[(end - start) + 1];
        _current_line_size = (end - start);
        copy_pos = 0;
    } else {
        // Expand the existing buffer
        new_line = new char[_current_line_size + (end - start) + 1];
        memcpy(new_line, _current_line, _current_line_size);
        copy_pos = _current_line_size;
        _current_line_size += (end - start);
        delete[] _current_line;
    }

    memcpy(&new_line[copy_pos], &src[start], end - start);
    _current_line = new_line;
}

/**
 * The main method implemented by this type. Reads the next full line out of the file. If
 * starting from a nonzero offset or ending before the end of the file, the first and last
 * lines respectively are skipped.
 * @return The next line, or nullptr if we are out of lines
 */
char* LineReader::readLine() {
    bool skip_line = _read_size == 0 && _file_start != 0;

    while (true) {
        // If we need more data from the file, read it
        if (_pos >= _buf_length) {
            // If the file is over, we're done
            if (_read_size >= _file_end - _file_start || feof(_file) || ferror(_file)) {
                char* line = _get_current_line();
                // If a -len was provided that is less than the file size, skip the last line
                if (_file_end != _file_size) {
                    if (line != nullptr) {
                        delete[] line;
                    }
                    return nullptr;
                }
                return line;
            }

            // Read up to sizeof buf data from the file
            size_t to_read = sizeof(_buf) / sizeof(char);
            // If we're up to the requested end position, only read as much as necessary
            if (_file_start + to_read > _file_end) {
                to_read -= (_file_start + to_read) - _file_end;
            }
            _buf_length = fread(_buf, sizeof(char), to_read, _file);
            _read_size += _buf_length;

            // Start processing at the beginning of the buffer
            _pos = 0;
            if (_buf_length == 0) {
                // If fread returned zero, go back to the start of the loop to check for
                // feof/ferror and otherwise try reading again
                continue;
            }
        }

        // Search for the next newline in the current buffer, and return the completed line
        // if we find one
        size_t start = _pos;
        for (size_t i = start; i < _buf_length; i++) {
            char c = _buf[i];
            if (c == '\n') {
                _append_current_line(_buf, start, i);
                _pos = i + 1;

                // If we started after 0, skip the first line we read as it may be partial
                if (skip_line) {
                    skip_line = false;
                    delete[] _get_current_line();
                    start = i + 1;
                    continue;
                }

                return _get_current_line();
            }
        }
        // If no newline found, loop around and read more file data
        _append_current_line(_buf, start, _buf_length);
        _pos = _buf_length;
    }
}

/**
 * Resets this reader. Goes back to the start position and prepares to read lines from there
 * again.
 */
void LineReader::reset() {
    if (_current_line != nullptr) {
        delete[] _current_line;
    }
    _current_line = nullptr;
    _current_line_size = 0;
    fseek(_file, _file_start, SEEK_SET);
    _pos = 0;
    _buf_length = 0;
    _read_size = 0;
}
