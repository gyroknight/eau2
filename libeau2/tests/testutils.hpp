/**
 * @file testutils.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <gtest/gtest.h>

#include <string>

#include "commondefs.hpp"

static const char col_types[] = {'I', 'B', 'D', 'S'};

#define EXPECT_SCHEMA_EQ(a, b) EXPECT_PRED2(isSchemaEq, a, b);
#define ASSERT_SCHEMA_EQ(a, b) ASSERT_PRED2(isSchemaEq, a, b);

[[maybe_unused]] static bool isSchemaEq(const Schema& a, const Schema& b) {
    if (a.width() != b.width()) return false;
    if (a.length() != b.length()) return false;

    for (size_t i = 0; i < a.width(); i++) {
        if (a.colName(i) != b.colName(i)) return false;
        if (a.colType(i) != b.colType(i)) return false;
    }

    for (size_t i = 0; i < a.length(); i++) {
        if (a.rowName(i) != b.rowName(i)) return false;
    }

    return true;
}

// same as above but for pointers
[[maybe_unused]] static bool isSchemaEq(const Schema* a, const Schema* b) {
    if (a != b) return false;
    if (a != nullptr) return isSchemaEq(*a, *b);

    return true;
}

class FixtureWithSmallDataFrame : public ::testing::Test {
   protected:
    DataFrame df;
    std::shared_ptr<Column<bool>> c1;
    std::shared_ptr<Column<int>> c2;
    std::shared_ptr<Column<ExtString>> c3;
    std::shared_ptr<Column<double>> c4;
    ExtString exts1;
    ExtString exts2;
    ExtString exts3;
    ExtString exts4;

    FixtureWithSmallDataFrame() {
        exts1 = std::make_shared<std::string>("abc");
        exts2 = std::make_shared<std::string>("def");
        exts3 = std::make_shared<std::string>("ghi");
        exts4 = std::make_shared<std::string>("ghi");
        c1->push_back(true);
        c1->push_back(false);
        c1->push_back(true);
        c2->push_back(10);
        c2->push_back(-5);
        c2->push_back(42);
        c3->push_back(exts1);
        c3->push_back(exts2);
        c3->push_back(exts3);
        c4->push_back(5.0);
        c4->push_back(0.0);
        c4->push_back(-10000.0);

        df.addCol(c1, std::make_shared<std::string>("boolcol"));
        df.addCol(c2, std::make_shared<std::string>("intcol"));
        df.addCol(c3, std::make_shared<std::string>("ExtStringcol"));
        df.addCol(c4, std::make_shared<std::string>("doublecol"));
    }
};