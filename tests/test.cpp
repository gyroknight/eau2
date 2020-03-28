#include <iostream>

#include "gtest/gtest.h"
#include "test_column.hpp"

int main(int argc, char** argv) {
    std::cout << "test" << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
