// lang::Cpp
/**
 * @file commondefs.hpp
 * @author Vincent Zhao
 * @brief Common type definitions used in DataFrames.
 */
#pragma once

#include <memory>
#include <string>
#include <vector>

// Common type definitions

class DataFrame;
template <typename T>
class Column;
class ColumnInterface;

// External string, memory management is handled by shared_ptr; when all
// references are deleted, memory will be freed
using ExtString = std::shared_ptr<std::string>;

using DFPtr = std::shared_ptr<DataFrame>;

template <typename T>
using ColPtr = std::shared_ptr<Column<T>>;

using ColIPtr = std::shared_ptr<ColumnInterface>;

using BStreamIter = std::vector<uint8_t>::iterator;