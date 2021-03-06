// lang::Cpp
/**
 * @file fielder.hpp
 * @author Vincent Zhao, Michael Hebert
 * @brief A visitor that performs some operation on each field in a Row.
 */
#pragma once

#include "commondefs.hpp"

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
    virtual void accept(ExtString s) = 0;

    /** Called when all fields have been seen. */
    virtual void done() {}
};
