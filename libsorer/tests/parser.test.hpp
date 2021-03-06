/**
 * @file parser.test.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * 
 * Lang::Cpp
 */

#include <gtest/gtest.h>

#include <assert.h>
#include <stdio.h>

#include "sorer/column.h"
#include "sorer/parser.h"

char* cwc_strdup(const char* src) {
    char* result = new char[strlen(src) + 1];
    strcpy(result, src);
    return result;
}

TEST(ParserTest, stringColumn) {
    ne::StringColumn* col = new ne::StringColumn();
    // Basic tests
    ASSERT_EQ(ne::ColumnType::STRING, col->getType());
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
    ne::IntegerColumn* col = new ne::IntegerColumn();
    // Basic int tests
    ASSERT_EQ(ne::ColumnType::INTEGER, col->getType());
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
    ne::FloatColumn* col = new ne::FloatColumn();
    // Basic float tests
    ASSERT_EQ(ne::ColumnType::FLOAT, col->getType());
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
    ne::BoolColumn* col = new ne::BoolColumn();
    // Basic bool tests
    ASSERT_EQ(ne::ColumnType::BOOL, col->getType());
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
    ne::StrSlice slice1{test_str, 17, 21};
    ASSERT_EQ(4, slice1.getLength());
    ASSERT_EQ('t', slice1.getChar(0));
    ASSERT_EQ('e', slice1.getChar(1));

    ne::StrSlice slice2{test_str, 5, 12};
    slice2.trim(' ');
    ASSERT_EQ('5', slice2.getChar(0));
    ASSERT_EQ('2', slice2.getChar(2));
    char* cstr = slice2.toCString();
    ASSERT_EQ(0, strcmp(cstr, "5.2"));
    delete[] cstr;
    ASSERT_EQ(5.2f, slice2.toFloat());

    ne::StrSlice slice3{test_str, 13, 16};
    ASSERT_EQ(100, slice3.toInt());

    ne::StrSlice slice4{"-128", 0, 4};
    ASSERT_EQ(-128, slice4.toInt());

    ne::StrSlice slice5{"+437896", 0, 5};
    ASSERT_EQ(4378, slice5.toInt());
}

class FileTest : public ::testing::Test {
   protected:
    FILE* file;
    size_t fsize;

    FileTest() : file(NULL), fsize(0) {}
    ~FileTest() {
        if (file) fclose(file);
    }

    void LoadFile(const char* filename) {
        if (file) fclose(file);

        file = fopen(filename, "r");

        ASSERT_FALSE(file == NULL);

        fseek(file, 0, SEEK_END);
        fsize = ftell(file);
        fseek(file, 0, SEEK_SET);
    }
};

TEST_F(FileTest, dataTest) {
    LoadFile("data.sor");
    
    ne::SorParser parser(file, 0, fsize, fsize);

    parser.guessSchema();
    parser.parseFile();

    ne::ColumnSet* set = parser.getColumnSet();

    ASSERT_EQ(ne::ColumnType::BOOL, set->getColumn(0)->getType());
    ASSERT_EQ(ne::ColumnType::INTEGER, set->getColumn(1)->getType());
    ASSERT_EQ(ne::ColumnType::STRING, set->getColumn(2)->getType());

    ne::BoolColumn* boolcol = dynamic_cast<ne::BoolColumn*>(set->getColumn(0));
    ne::IntegerColumn* intcol = dynamic_cast<ne::IntegerColumn*>(set->getColumn(1));
    ne::StringColumn* strcol = dynamic_cast<ne::StringColumn*>(set->getColumn(2));

    ASSERT_EQ(false, boolcol->getEntry(0));
    ASSERT_EQ(true, boolcol->getEntry(1));
    ASSERT_EQ(true, boolcol->getEntry(2));

    ASSERT_EQ(23, intcol->getEntry(0));
    ASSERT_EQ(12, intcol->getEntry(1));
    ASSERT_EQ(1, intcol->getEntry(2));

    ASSERT_STREQ("hi", strcol->getEntry(0));
    ASSERT_FALSE(strcol->isEntryPresent(1));
    ASSERT_FALSE(strcol->isEntryPresent(2));
}
