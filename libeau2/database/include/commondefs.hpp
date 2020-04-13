// lang::Cpp
/**
 * @file commondefs.hpp
 * @author Vincent Zhao
 * @brief Common type definitions used in DataFrames.
 */
#pragma once

#include <memory>
#include <string>

// Common type definitions

// External string, memory management is handled by shared_ptr; when all
// references are deleted, memory will be freed
using ExtString = std::shared_ptr<std::string>;