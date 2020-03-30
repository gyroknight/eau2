// lang::CwC
#pragma once

#include <memory>
#include <string>

using ExtString = std::shared_ptr<std::string>;

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
    virtual void accept(std::shared_ptr<bool> b) = 0;
    virtual void accept(std::shared_ptr<double> d) = 0;
    virtual void accept(std::shared_ptr<int> i) = 0;
    virtual void accept(ExtString s) = 0;

    /** Called when all fields have been seen. */
    virtual void done() {}
};
