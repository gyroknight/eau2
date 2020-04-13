#include <unistd.h>

#include "sorer/object.h"

namespace ne {

/**
 * Constructs a new Object. The object does not contain anything.
 */
Object::Object() {}
/**
 * Destructs this object.
 */
Object::~Object() {}

/**
 * Returns a number suitable as a hash code for this object.
 * In this implementation, this is equivalent to the address of
 * this object in memory as a size_t.
 */
size_t Object::hash() { return reinterpret_cast<size_t>(this); }

/**
 * Checks if this object is equal to another object.
 * In this base implementation, checks if these objects refer to exactly the same place in
 * memory.
 */
bool Object::equals(Object* other) { return this == other; }

} // namespace ne