// Lang::CwC
#pragma once

#include <unistd.h>

/**
 * Base class for all objects in this CwC code.
 * Contains some common methods for all Objects.
 */
class Object {
   public:
    /**
     * Constructs a new Object. The object does not contain anything.
     */
    Object() {}
    /**
     * Destructs this object.
     */
    virtual ~Object() {}

    /**
     * Returns a number suitable as a hash code for this object.
     * In this implementation, this is equivalent to the address of
     * this object in memory as a size_t.
     */
    virtual size_t hash() { return reinterpret_cast<size_t>(this); }

    /**
     * Checks if this object is equal to another object.
     * In this base implementation, checks if these objects refer to exactly the same place in
     * memory.
     */
    virtual bool equals(Object* other) { return this == other; }
};
