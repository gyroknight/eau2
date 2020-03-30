// lang::Cpp
#pragma once
#include <gtest/gtest.h>

#include <cstdlib>

#include "column.hpp"

namespace {

// Fixture with some useful methods - this can be inherited from and used to set up several different tests
class IntColumnTest : public testing::Test {
   protected:
    std::shared_ptr<Column<int>> ic;

    // Helper method to add random ints to the column
    void AddRandom(size_t n) {
        while (n--) ic->push_back(rand());
    }

    // Helper method to add sequential ints to the column
    void AddSequential(size_t n) {
        for (size_t i = 0; i < n; i++) ic->push_back(static_cast<int>(i));
    }
};

// Fixture with an empty Column<int>
class IntColumnEmpty : public IntColumnTest {
   public:
    IntColumnEmpty() { ic = std::make_shared<Column<int>>(); }
};

// test size of empty
TEST_F(IntColumnEmpty, size_empty) { EXPECT_EQ(0u, ic->size()); }

// test pushing back one elem
TEST_F(IntColumnEmpty, push_back_one) {
    ic->push_back(4);

    EXPECT_EQ(1u, ic->size());
}

// test pushing back a lot of elements
TEST_F(IntColumnEmpty, push_back_size_big) {
    AddRandom(1000000);

    EXPECT_EQ(1000000u, ic->size());
}

// test get on a small size
TEST_F(IntColumnEmpty, get_small) {
    ic->push_back(8);
    ic->push_back(-13);
    ic->push_back(0);
    ic->push_back(7);
    ic->push_back(8);
    ic->push_back(1);
    ic->push_back(10000);

    EXPECT_EQ(7u, ic->size());

    EXPECT_EQ(8, ic->get(0));
    EXPECT_EQ(-13, ic->get(1));
    EXPECT_EQ(0, ic->get(2));
    EXPECT_EQ(7, ic->get(3));
    EXPECT_EQ(8, ic->get(4));
    EXPECT_EQ(1, ic->get(5));
    EXPECT_EQ(10000, ic->get(6));
}

// test get on a large size
TEST_F(IntColumnEmpty, get_big) {
    AddSequential(100000);

    for (size_t i = 0; i < 100000; i++) ASSERT_EQ(i, ic->get(i));
}

// test c str on empty
TEST_F(IntColumnEmpty, c_str_zero) { ASSERT_STREQ("", ic->str().c_str()); }

// test c str on single elem
TEST_F(IntColumnEmpty, c_str_one) {
    ic->push_back(4);

    ASSERT_STREQ("4", ic->str().c_str());
}

// test c str on several
TEST_F(IntColumnEmpty, c_str_many) {
    AddSequential(10);

    ASSERT_STREQ("0, 1, 2, 3, 4, 5, 6, 7, 8, 9", ic->str().c_str());
}

// test set with few elem
TEST_F(IntColumnEmpty, set_few) {
    AddSequential(10);

    ASSERT_STREQ("0, 1, 2, 3, 4, 5, 6, 7, 8, 9", ic->str().c_str());

    ic->set(0, 100);
    ic->set(4, 42);
    ic->set(1, 3);
    ic->set(9, -10);
    ic->set(9, 10000);

    ASSERT_STREQ("100, 3, 2, 3, 42, 5, 6, 7, 8, 10000", ic->str().c_str());
}

// test set with many elem
TEST_F(IntColumnEmpty, set_many) {
    AddSequential(100000);

    ic->set(0, -5);
    ic->set(99998, 42);
    ic->set(99999, 41);

    EXPECT_EQ(-5, ic->get(0));
    EXPECT_EQ(42, ic->get(99998));
    EXPECT_EQ(41, ic->get(99999));
}

class IntColumnEqual : public IntColumnTest {
   public:
    Column<int> ic2;

    IntColumnEqual()
        : ic2{1, 2, 3} {
        ic = std::make_shared<Column<int>>(std::initializer_list<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
        ic2.push_back(4);
        ic2.push_back(5);
        ic2.push_back(6);
        ic2.push_back(7);
        ic2.push_back(8);
        ic2.push_back(9);
        ic2.push_back(10);
    }
};

// Testing int columns constructed with initializer lists

// Fixture with a single element column
class IntColumnVaSingle : public IntColumnTest {
   public:
    IntColumnVaSingle() { ic = std::make_shared<Column<int>>(std::initializer_list<int>{42}); }
};

// Fixture with a 250-element column
class IntColumnVaMany : public IntColumnTest {
   public:
    IntColumnVaMany() {
        ic = std::make_shared<Column<int>>(std::initializer_list<int>{
            702, 138, -859, 95, -601, 192, 990, -939, 815, -747, -609,
            -392, 642, 866, -459, 989, 744, -269, 2, -469, 766, -929, -95, -14,
            58, 629, 236, -292, 117, 944, 424, 969, -140, -940, -621, 221, 135,
            634, -918, 882, -363, 20, 719, -386, -87, -32, 777, -687, -823, 72,
            -559, 366, 299, -824, 972, 730, -577, 904, -252, -301, 263, 934, 35,
            -134, -289, 619, -744, 771, 15, 653, -161, 1000, 386, -32, -46, 711,
            339, -482, -901, 574, -976, -471, 918, 607, 448, 347, 161, 494,
            -614, -985, 916, -756, 753, -942, 210, -552, 150, 598, 422, -803,
            580, 114, 515, -195, 208, 99, 720, -487, -737, -46, 467, 909, -578,
            -470, 839, -81, 665, 249, -468, 266, -227, -962, 894, -283, -132,
            354, -437, 699, 194, -349, -649, -121, 762, -720, -208, -98, 807,
            -244, -599, -703, 877, 661, 236, 836, -440, 25, 954, -620, 553,
            -177, 45, -271, -843, -518, -298, 325, -410, 639, -788, 518, -59,
            534, -894, 71, -571, -909, -202, -893, 731, 879, 169, -776, 675,
            -100, 498, -448, 109, -826, -152, 538, -164, 416, 80, 751, -334,
            162, 935, 310, -189, -334, 336, -529, 241, -975, 846, -409, 77,
            -735, -7, -69, -354, 835, -349, 252, -811, 736, 349, 685, -852,
            -282, 799, 281, 688, -314, -241, 800, -860, -825, 396, 824, 844,
            -430, 869, -780, -117, 458, 472, -335, -931, 220, -400, 492, -223,
            878, 855, 290, -877, -256, 106, -360, -81, -413, 431, 579, 731, -6,
            -49, 281, 284, 608});
    }
};

// test methods on a constructor with one element
TEST_F(IntColumnVaSingle, construct_1) {
    EXPECT_EQ(1u, ic->size());
    EXPECT_EQ(42, ic->get(0));
    ASSERT_STREQ("42", ic->str().c_str());
}

// test methods with many elem constructor
TEST_F(IntColumnVaMany, construct_x) {
    EXPECT_EQ(250u, ic->size());

    EXPECT_EQ(702, ic->get(0));
    EXPECT_EQ(138, ic->get(1));
    EXPECT_EQ(-859, ic->get(2));
    EXPECT_EQ(580, ic->get(100));
    EXPECT_EQ(284, ic->get(248));
    EXPECT_EQ(608, ic->get(249));

    ASSERT_STREQ(
        "702, 138, -859, 95, -601, 192, 990, -939, 815, -747, -609, -392, 642, "
        "866, "
        "-459, 989, 744, -269, 2, -469, 766, -929, -95, -14, 58, 629, 236, "
        "-292, 117, "
        "944, 424, 969, -140, -940, -621, 221, 135, 634, -918, 882, -363, 20, "
        "719, "
        "-386, -87, -32, 777, -687, -823, 72, -559, 366, 299, -824, 972, 730, "
        "-577, "
        "904, -252, -301, 263, 934, 35, -134, -289, 619, -744, 771, 15, 653, "
        "-161, "
        "1000, 386, -32, -46, 711, 339, -482, -901, 574, -976, -471, 918, 607, "
        "448, "
        "347, 161, 494, -614, -985, 916, -756, 753, -942, 210, -552, 150, 598, "
        "422, "
        "-803, 580, 114, 515, -195, 208, 99, 720, -487, -737, -46, 467, 909, "
        "-578, "
        "-470, 839, -81, 665, 249, -468, 266, -227, -962, 894, -283, -132, "
        "354, -437, "
        "699, 194, -349, -649, -121, 762, -720, -208, -98, 807, -244, -599, "
        "-703, 877, "
        "661, 236, 836, -440, 25, 954, -620, 553, -177, 45, -271, -843, -518, "
        "-298, "
        "325, -410, 639, -788, 518, -59, 534, -894, 71, -571, -909, -202, "
        "-893, 731, "
        "879, 169, -776, 675, -100, 498, -448, 109, -826, -152, 538, -164, "
        "416, 80, "
        "751, -334, 162, 935, 310, -189, -334, 336, -529, 241, -975, 846, "
        "-409, 77, "
        "-735, -7, -69, -354, 835, -349, 252, -811, 736, 349, 685, -852, -282, "
        "799, "
        "281, 688, -314, -241, 800, -860, -825, 396, 824, 844, -430, 869, "
        "-780, -117, "
        "458, 472, -335, -931, 220, -400, 492, -223, 878, 855, 290, -877, "
        "-256, 106, "
        "-360, -81, -413, 431, 579, 731, -6, -49, 281, 284, 608",
        ic->str().c_str());
}

// test pushing one elemen with one constructed
TEST_F(IntColumnVaSingle, push_back_one_1) {
    ic->push_back(4);

    EXPECT_EQ(2u, ic->size());
    EXPECT_EQ(4, ic->get(1));
}

// test pushing one elem with many cons
TEST_F(IntColumnVaMany, push_back_one_x) {
    ic->push_back(4);

    EXPECT_EQ(251u, ic->size());
    EXPECT_EQ(4, ic->get(250));
}

TEST_F(IntColumnVaSingle, push_back_size_big_1) {
    AddRandom(1000000);

    EXPECT_EQ(1000001u, ic->size());
}

TEST_F(IntColumnVaMany, push_back_size_big_x) {
    AddRandom(1000000);

    EXPECT_EQ(1000250u, ic->size());
}

TEST_F(IntColumnVaSingle, get_small_1) {
    ic->push_back(8);
    ic->push_back(-13);
    ic->push_back(0);
    ic->push_back(7);
    ic->push_back(8);
    ic->push_back(1);
    ic->push_back(10000);

    EXPECT_EQ(8u, ic->size());

    EXPECT_EQ(42, ic->get(0));  // already there
    EXPECT_EQ(8, ic->get(1));
    EXPECT_EQ(-13, ic->get(2));
    EXPECT_EQ(0, ic->get(3));
    EXPECT_EQ(7, ic->get(4));
    EXPECT_EQ(8, ic->get(5));
    EXPECT_EQ(1, ic->get(6));
    EXPECT_EQ(10000, ic->get(7));
}

TEST_F(IntColumnVaMany, get_small_x) {
    ic->push_back(8);
    ic->push_back(-13);
    ic->push_back(0);
    ic->push_back(7);
    ic->push_back(8);
    ic->push_back(1);
    ic->push_back(10000);

    EXPECT_EQ(257u, ic->size());

    EXPECT_EQ(8, ic->get(250));
    EXPECT_EQ(-13, ic->get(251));
    EXPECT_EQ(0, ic->get(252));
    EXPECT_EQ(7, ic->get(253));
    EXPECT_EQ(8, ic->get(254));
    EXPECT_EQ(1, ic->get(255));
    EXPECT_EQ(10000, ic->get(256));
}

TEST_F(IntColumnVaSingle, get_big_1) {
    AddSequential(100000);

    EXPECT_EQ(42, ic->get(0));

    for (size_t i = 1; i < 100001; i++) ASSERT_EQ(i - 1, ic->get(i));
}
TEST_F(IntColumnVaMany, get_big_x) {
    AddSequential(100000);

    EXPECT_EQ(702, ic->get(0));
    EXPECT_EQ(138, ic->get(1));
    EXPECT_EQ(-859, ic->get(2));
    EXPECT_EQ(580, ic->get(100));
    EXPECT_EQ(284, ic->get(248));
    EXPECT_EQ(608, ic->get(249));

    for (size_t i = 250; i < 100250; i++) ASSERT_EQ(i - 250, ic->get(i));
}

// test set with few elem
TEST_F(IntColumnVaSingle, set_few) {
    AddSequential(10);

    ASSERT_STREQ("42, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9", ic->str().c_str());

    ic->set(0, 100);
    ic->set(4, 42);
    ic->set(1, 3);
    ic->set(9, -10);
    ic->set(9, 10000);

    ASSERT_STREQ("100, 3, 1, 2, 42, 4, 5, 6, 7, 10000, 9", ic->str().c_str());
}

// test set with few elem
TEST_F(IntColumnVaMany, set_few) {
    AddSequential(10);

    ic->set(0, 100);
    ic->set(40, 42);
    ic->set(17, 3);
    ic->set(90, -10);
    ic->set(249, 10000);
    ic->set(255, 75);

    ASSERT_EQ(100, ic->get(0));
    ASSERT_EQ(42, ic->get(40));
    ASSERT_EQ(3, ic->get(17));
    ASSERT_EQ(-10, ic->get(90));
    ASSERT_EQ(10000, ic->get(249));
    ASSERT_EQ(0, ic->get(250));
    ASSERT_EQ(1, ic->get(251));
    ASSERT_EQ(2, ic->get(252));
    ASSERT_EQ(75, ic->get(255));
    ASSERT_EQ(6, ic->get(256));
}

// test set with many elem
TEST_F(IntColumnVaSingle, set_many) {
    AddSequential(100000);

    ic->set(0, -5);
    ic->set(1, 6);
    ic->set(99999, 42);
    ic->set(100000, 41);

    EXPECT_EQ(-5, ic->get(0));
    EXPECT_EQ(6, ic->get(1));
    EXPECT_EQ(42, ic->get(99999));
    EXPECT_EQ(41, ic->get(100000));
}

// test set with many elem
TEST_F(IntColumnVaMany, set_many) {
    AddSequential(100000);

    ic->set(0, -5);
    ic->set(1, 6);
    ic->set(100, -100);
    ic->set(99999, 42);
    ic->set(100000, 41);

    EXPECT_EQ(-5, ic->get(0));
    EXPECT_EQ(6, ic->get(1));
    EXPECT_EQ(-100, ic->get(100));
    EXPECT_EQ(42, ic->get(99999));
    EXPECT_EQ(41, ic->get(100000));
}

}  // namespace
