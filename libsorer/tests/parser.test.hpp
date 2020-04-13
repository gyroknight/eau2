// Lang::CwC
//

#include <gtest/gtest.h>

#include <assert.h>
#include <stdio.h>

#include "parser.h"

char* cwc_strdup(const char* src) {
    char* result = new char[strlen(src) + 1];
    strcpy(result, src);
    return result;
}

TEST(ParserTest, stringColumn) {
    StringColumn* col = new StringColumn();
    // Basic tests
    ASSERT_EQ(ColumnType::STRING, col->getType());
    col->append(cwc_strdup("test1"));
    col->appendMissing();
    col->append(cwc_strdup("test2"));
    ASSERT_EQ(3, col->getLength());
    ASSERT_TRUE(col->isEntryPresent(0));
    ASSERT_TRUE(!col->isEntryPresent(1));
    ASSERT_TRUE(col->isEntryPresent(2));
    ASSERT_EQ(0, strcmp(col->getEntry(0), "test1"));
    ASSERT_EQ(0, strcmp(col->getEntry(2), "test2"));

    // Use StrColumn to run a large test to make sure arraylists expand correctly
    for (size_t i = 0; i < 1024; i++) {
        col->append(cwc_strdup("test1024"));
    }

    ASSERT_EQ(1027, col->getLength());
    ASSERT_EQ(0, strcmp(col->getEntry(100), "test1024"));
    delete col;
}

TEST(ParserTest, intColumn) {
    IntegerColumn* col = new IntegerColumn();
    // Basic int tests
    ASSERT_EQ(ColumnType::INTEGER, col->getType());
    col->appendMissing();
    col->appendMissing();
    col->append(1);
    col->append(2);
    ASSERT_EQ(4, col->getLength());
    ASSERT_TRUE(col->isEntryPresent(3));
    ASSERT_TRUE(!col->isEntryPresent(0));
    ASSERT_EQ(2, col->getEntry(3));
    delete col;
}

TEST(ParserTest, floatColumn) {
    FloatColumn* col = new FloatColumn();
    // Basic float tests
    ASSERT_EQ(ColumnType::FLOAT, col->getType());
    col->append(4500.0);
    col->appendMissing();
    col->appendMissing();
    ASSERT_EQ(3, col->getLength());
    ASSERT_TRUE(col->isEntryPresent(0));
    ASSERT_TRUE(!col->isEntryPresent(2));
    ASSERT_EQ(4500.0, col->getEntry(0));
    delete col;
}

TEST(ParserTest, boolColumn) {
    BoolColumn* col = new BoolColumn();
    // Basic bool tests
    ASSERT_EQ(ColumnType::BOOL, col->getType());
    col->append(true);
    col->append(false);
    col->appendMissing();
    ASSERT_EQ(3, col->getLength());
    ASSERT_TRUE(!col->isEntryPresent(2));
    ASSERT_TRUE(col->isEntryPresent(1));
    ASSERT_EQ(false, col->getEntry(1));
    delete col;
}

TEST(ParserTest, strSlice) {
    const char* test_str = "abcde   5.2  100 test";
    StrSlice slice1{test_str, 17, 21};
    ASSERT_EQ(4, slice1.getLength());
    ASSERT_EQ('t', slice1.getChar(0));
    ASSERT_EQ('e', slice1.getChar(1));

    StrSlice slice2{test_str, 5, 12};
    slice2.trim(' ');
    ASSERT_EQ('5', slice2.getChar(0));
    ASSERT_EQ('2', slice2.getChar(2));
    char* cstr = slice2.toCString();
    ASSERT_EQ(0, strcmp(cstr, "5.2"));
    delete[] cstr;
    ASSERT_EQ(5.2f, slice2.toFloat());

    StrSlice slice3{test_str, 13, 16};
    ASSERT_EQ(100, slice3.toInt());

    StrSlice slice4{"-128", 0, 4};
    ASSERT_EQ(-128, slice4.toInt());

    StrSlice slice5{"+437896", 0, 5};
    ASSERT_EQ(4378, slice5.toInt());
}