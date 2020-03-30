#include <memory>

#include "column.hpp"
#include "dataframe.hpp"
#include "gtest/gtest.h"

namespace {

// Define test fixture class template
template <typename T>
class ColumnTest : public testing::Test {
   protected:
    // TODO(mike) test via interface instead
    std::shared_ptr<Column<T>> _col;

    // The ctor creates a column<T>
    ColumnTest() : _col(std::make_shared<Column<T>>()) {}

    // Adds a bunch of random objects
    void AddRandom(size_t n);

    // Adds a bunch of sequential objects
    void AddSequential(size_t n);
};

template <>
void ColumnTest<int>::AddRandom(size_t n) {
    while (n--) _col->push_back(rand());
}

template <>
void ColumnTest<int>::AddSequential(size_t n) {
    for (size_t i = 0; i < n; i++) _col->push_back(static_cast<int>(i));
}

using Types = ::testing::Types<int>;  // todo add other types to this list <int,
                                      // bool, double, std::string>
TYPED_TEST_SUITE(ColumnTest, Types);

TYPED_TEST(ColumnTest, size_empty) { EXPECT_EQ(0u, this->_col->size()); }

// test pushing back one elem
TYPED_TEST(ColumnTest, push_back_one) {
    this->_col->push_back(4);

    EXPECT_EQ(1u, this->_col->size());
}

// test pushing back a lot of elements
TYPED_TEST(ColumnTest, push_back_size_big) {
    this->AddRandom(1000000);

    EXPECT_EQ(1000000u, this->_col->size());
}

// test get on a small size
TYPED_TEST(ColumnTest, get_small) {
    this->_col->push_back(8);
    this->_col->push_back(-13);
    this->_col->push_back(0);
    this->_col->push_back(7);
    this->_col->push_back(8);
    this->_col->push_back(1);
    this->_col->push_back(10000);

    EXPECT_EQ(7u, this->_col->size());

    EXPECT_EQ(8, this->_col->get(0));
    EXPECT_EQ(-13, this->_col->get(1));
    EXPECT_EQ(0, this->_col->get(2));
    EXPECT_EQ(7, this->_col->get(3));
    EXPECT_EQ(8, this->_col->get(4));
    EXPECT_EQ(1, this->_col->get(5));
    EXPECT_EQ(10000, this->_col->get(6));
}

// test get on a large size
TYPED_TEST(ColumnTest, get_big) {
    this->AddSequential(100000);

    for (size_t i = 0; i < 100000; i++) ASSERT_EQ(i, this->_col->get(i));
}

// test c str on empty
// TYPED_TEST(ColumnTest, c_str_zero) { ASSERT_STREQ("", this->_col->c_str()); }

// test c str on single elem
// TYPED_TEST(ColumnTest, c_str_one) {
//     this->_col->push_back(4);

//     ASSERT_STREQ("4", this->_col->c_str());
// }

// test c str on several
// TYPED_TEST(ColumnTest, c_str_many) {
//     this->AddSequential(10);

//     ASSERT_STREQ("0, 1, 2, 3, 4, 5, 6, 7, 8, 9", this->_col->c_str());
// }

// test set with few elem
// TYPED_TEST(ColumnTest, set_few) {
//     this->AddSequential(10);

//     ASSERT_STREQ("0, 1, 2, 3, 4, 5, 6, 7, 8, 9", this->_col->c_str());

//     this->_col->set(0, 100);
//     this->_col->set(4, 42);
//     this->_col->set(1, 3);
//     this->_col->set(9, -10);
//     this->_col->set(9, 10000);

//     ASSERT_STREQ("100, 3, 2, 3, 42, 5, 6, 7, 8, 10000", this->_col->c_str());
// }

// test set with many elem
TYPED_TEST(ColumnTest, set_many) {
    this->AddSequential(100000);

    this->_col->set(0, -5);
    this->_col->set(99998, 42);
    this->_col->set(99999, 41);

    EXPECT_EQ(-5, this->_col->get(0));
    EXPECT_EQ(42, this->_col->get(99998));
    EXPECT_EQ(41, this->_col->get(99999));
}

// push_back(bool)
// push_back(float)
// push_back(String*)
// TYPED_TEST(ColumnTest, push_back_wrong_types) {
//     std::string s = "hi";

//     this->_col->push_back(true);
//     this->_col->push_back(s);
//     this->_col->push_back(1.0f);

//     ASSERT_EQ(0, this->_col->size());
// }

// get_type
TYPED_TEST(ColumnTest, get_type) {
    EXPECT_EQ('I', Schema::colToType(*this->_col));
}

}  // namespace