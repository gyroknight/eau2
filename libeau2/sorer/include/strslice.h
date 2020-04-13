#pragma once

#include "object.h"

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
