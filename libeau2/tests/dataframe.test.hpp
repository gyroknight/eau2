#pragma once

#include <gtest/gtest.h>

#include "dataframe.hpp"

// Fixture with some useful methods - this can be inherited from and used to set up several different tests
class DataframeTest : public testing::Test {
   public:
};

// Ctor from schema
TEST_F(DataframeTest, schema_ctor) {
    Schema schema()
}