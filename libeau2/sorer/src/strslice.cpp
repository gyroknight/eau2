#include <cassert>
#include <cstring>
#include <cstdlib>

#include "strslice.h"

/**
 * Creates a new StrSlice.
 * @param str The C-style-string to slice from. Must be valid during the lifetime of this slice
 * @param start The starting index
 * @param end The ending index
 */
StrSlice::StrSlice(const char* str, size_t start, size_t end) : Object() {
    _start = start;
    _end = end;
    _str = str;
}

/**
 * @return The length of this slice
 */
size_t StrSlice::getLength() { return _end - _start; }
/**
 * @return A non-null-terminated pointer to the chars in this slice.
 */
const char* StrSlice::getChars() { return &_str[_start]; }
/**
 * Gets the char at the given index.
 * @param which The index. Must be < getLength()
 * @return The char
 */
char StrSlice::getChar(size_t which) {
    assert(which < _end - _start);
    return _str[_start + which];
}

/**
 * Updates the bounds of this slice to exclude instances of the given char at the beginning or
 * end. May shorten the length of this slice.
 * @param c The char to remove from the beginning and end
 */
void StrSlice::trim(char c) {
    while (_start < _end && _str[_start] == c) {
        _start++;
    }
    while (_start < _end && _str[_end - 1] == c) {
        _end--;
    }
}

/**
 * Allocates a new null terminated string and copies the contents of this slice to it.
 * @return The new string. Caller must free
 */
char* StrSlice::toCString() {
    size_t length = _end - _start;
    char* sliceCopy = new char[length + 1];
    memcpy(sliceCopy, getChars(), length);
    sliceCopy[length] = '\0';
    return sliceCopy;
}

/**
 * Parses the contents of this slice as an int.
 * @return An int corresponding to the digits in this slice.
 */
int StrSlice::toInt() {
    // Roll a custom integer parsing function to avoid having to allocate a new null-terminated
    // string for atoi and friends.
    long result = 0;
    bool is_negative = false;
    for (size_t i = _start; i < _end; i++) {
        char c = _str[i];
        if (i == _start && c == '-') {
            is_negative = true;
            continue;
        } else if (i == _start && c == '+') {
            continue;
        } else if (c >= '0' && c <= '9') {
            result = result * 10 + (c - '0');
        } else {
            break;
        }
    }
    return is_negative ? -result : result;
}

/**
 * Parses the contents of this slice as a float.
 * @return The float
 */
float StrSlice::toFloat() {
    // It's hard to roll a float parsing function by hand, so bite the bullet and allocate a
    // null-terminated string for atof.
    char* cstr = toCString();
    float result = atof(cstr);
    delete[] cstr;
    return result;
}


