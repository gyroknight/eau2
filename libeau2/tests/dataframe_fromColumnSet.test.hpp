/**
 * @file dataframe_fromColumnSet.test.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <queue>

#include "dataframe.hpp"
#include "kill.hpp"
#include "kvnet.hpp"
#include "kvstore.hpp"
#include "message.hpp"
#include "sorer/column.h"
#include "testutils.hpp"

// Necessary to avoid segfaults with deleting string literals
char* cwc_strdup(const char* src) {
    char* result = new char[strlen(src) + 1];
    strcpy(result, src);
    return result;
}

// test fromColumnSet method

// Currently commented out because the kvnet is not responding to kill messages
// correctly.
/*
TEST(DataFrameTest, fromColumn) {
    KVNetMock net;
    KVStore kv(net);
    Key k("dataf", 0);

    ne::ColumnSet set(3);
    set.initializeColumn(0, ne::ColumnType::BOOL);
    set.initializeColumn(1, ne::ColumnType::INTEGER);
    set.initializeColumn(2, ne::ColumnType::STRING);

    ne::BoolColumn* col1 = dynamic_cast<ne::BoolColumn*>(set.getColumn(0));
    ne::IntegerColumn* col2 =
dynamic_cast<ne::IntegerColumn*>(set.getColumn(1)); ne::StringColumn* col3 =
dynamic_cast<ne::StringColumn*>(set.getColumn(2));

    // build columns
    col1->append(true);
    col1->append(true);
    col1->append(false);

    col2->append(42);
    col2->append(-5);
    col2->append(0);

    col3->append(cwc_strdup("hello"));
    col3->append(cwc_strdup("goodbye"));
    col3->append(cwc_strdup("yes"));

    DataFrame::fromColumnSet(&k, &kv, &set);

    // net.send(std::make_shared<Kill>(0, 0));

    auto df = kv.waitAndGet(k);

    ASSERT_EQ(true, df->getBool(0, 0));
    ASSERT_EQ(-5, df->getInt(1, 1));
    ASSERT_TRUE(strcmp("yes", df->getString(2, 2)->c_str()));
}
*/
