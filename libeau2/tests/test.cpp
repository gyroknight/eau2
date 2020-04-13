#include <iostream>

#include "gtest/gtest.h"

#include "column_int.test.hpp"
#include "schema.test.hpp"
#include "row-fielder.test.hpp"
// #include "kvstore.test.hpp" // segfaults
// #include "dataframe.test.hpp" // Not working for some reason
#include "dataframe_fromColumnSet.test.hpp"

int main(int argc, char** argv) {
    std::cout << "test" << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
