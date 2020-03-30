#include <iostream>

#include "gtest/gtest.h"
#include "column.test.hpp"

int main(int argc, char** argv) {
    std::cout << "test" << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
