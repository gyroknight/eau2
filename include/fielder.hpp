// lang::CwC
#pragma once

#include <string>

/*****************************************************************************
 * Fielder::
 * A field vistor invoked by Row.
 */
class Fielder {
   public:
    /** Called before visiting a row, the argument is the row offset in the
      dataframe. */
    virtual void start(size_t r) {}

    /** Called for fields of the argument's type with the value of the field. */
    virtual void accept(bool b) = 0;
    virtual void accept(double d) = 0;
    virtual void accept(int i) = 0;
    virtual void accept(std::string& s) = 0;

    /** Called when all fields have been seen. */
    virtual void done() {}
};
