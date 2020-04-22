/**
 * @file testutils.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <cstring>

#include "gtest/gtest.h"
#include "schema.hpp"
#include "testutils.hpp"

namespace {

/* fixtures */

// provides a parameter to do something with
class SchemaEmpty : public testing::TestWithParam<const char*> {
   public:
    size_t len;

    SchemaEmpty() : len(strlen(GetParam())) {}
};

// INSTANTIATE_TEST_SUITE_P(VariedString, SchemaEmpty,
//     testing::Values("", "I", "B", "D", "S", "IBDSSIBSDD",
//         "SDBISBDISBDISBDBBBBDBISDISIIIDIBBSISIISISBDBSIDBIS"));

// creates a schema with the given column types
class SchemaTestString : public testing::TestWithParam<const char*> {
   public:
    Schema sc;
    size_t len;

    SchemaTestString() : sc(GetParam()), len(strlen(GetParam())) {}
};

INSTANTIATE_TEST_SUITE_P(
    VariedString, SchemaTestString,
    testing::Values("", "I", "B", "D", "S", "IBDSSIBSDD",
                    "SDBISBDISBDISBDBBBBDBISDISIIIDIBBSISIISISBDBSIDBIS"));

class SchemaTestNumber : public testing::TestWithParam<int> {
   public:
    SchemaTestNumber() {}
};

// INSTANTIATE_TEST_SUITE_P(VariedString, SchemaTestNumber,
//     testing::Values(0, 1, 2, 5, 10, 100, 1000, 100000));

/* test constructors */

TEST(SchemaEmpty, cons_empty) {
    Schema sc;

    ASSERT_EQ(0, sc.width());
    ASSERT_EQ(0, sc.length());
}

TEST_P(SchemaTestString, cons_string) {
    ASSERT_EQ(len, sc.width());
    ASSERT_EQ(0, sc.length());

    for (size_t i = 0; i < len; i++) {
        ASSERT_EQ(GetParam()[i], sc.colType(i));
    }
}

TEST_P(SchemaTestString, cons_copy) {
    Schema cp(sc);

    ASSERT_EQ(len, cp.width());
    ASSERT_EQ(0, cp.length());

    for (size_t i = 0; i < len; i++) {
        ASSERT_EQ(GetParam()[i], cp.colType(i));
    }
}

class SchemaTest : public testing::Test {
   public:
    // appends an int to a string
    std::string str(std::string s, int n) {
        return s.append(std::to_string(n));
    }
};

// test add_col
TEST_F(SchemaTest, add_col) {
    Schema sc;

    for (int i = 0; i < 10000; i++) {
        sc.addCol(col_types[i % 4],
                  std::make_shared<std::string>(i % 2 ? "" : str("name", i)));
        ASSERT_EQ(i + 1, sc.width());
    }

    for (int i = 0; i < 10000; i++) {
        ASSERT_STREQ(i % 2 ? "" : str("name", i).c_str(),
                     sc.colName(i).c_str());
    }
}

// test addRow
TEST_F(SchemaTest, addRow) {
    Schema sc;

    for (int i = 0; i < 10000; i++) {
        sc.addRow(std::make_shared<std::string>(i % 2 ? "" : str("name", i)));
        ASSERT_EQ(i + 1, sc.length());
    }

    for (int i = 0; i < 10000; i++) {
        ASSERT_STREQ(i % 2 ? "" : str("name", i).c_str(),
                     sc.rowName(i).c_str());
    }
}

/* test col_idx */
TEST_F(SchemaTest, col_idx) {
    Schema sc;

    for (int i = 0; i < 1000; i++) {
        sc.addCol(col_types[i % 4],
                  std::make_shared<std::string>(str("name", i)));
    }

    for (int i = 0; i < 1000; i++) {
        ASSERT_EQ(i, sc.colIdx(("name" + std::to_string(i)).c_str()));
    }
}

/* test row_idx */
TEST_F(SchemaTest, row_idx) {
    Schema sc;

    for (int i = 0; i < 1000; i++) {
        sc.addRow(std::make_shared<std::string>(str("name", i)));
    }

    for (int i = 0; i < 1000; i++) {
        ASSERT_EQ(i, sc.rowIdx(("name" + std::to_string(i)).c_str()));
    }
}

// width and height tested with above methods

}  // namespace
