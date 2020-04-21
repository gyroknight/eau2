/**
 * @file serializer.test.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include "gtest/gtest.h"

#include "serial.hpp"
#include "serializer.hpp"

#include <string>
#include <memory>
#include <vector>

namespace {

TEST(SerializerTest, basic) {
    Serializer ss;

    auto bytes = ss.generate();


}

} // namespace