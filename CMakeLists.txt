cmake_minimum_required(VERSION 3.2.2)

project(eau2 VERSION 0.01
             DESCRIPTION "Distributed key-value store"
             LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(libeau2)
add_subdirectory(tests)
# add_subdirectory(demo)

# PHONY target to run tests
add_custom_target(test
    COMMAND ./tests/test_main
    DEPENDS test_main
)

# PHONY target to run valgrind
# add_custom_target(valgrind
#     COMMAND valgrind --leak-check=full --tool=memcheck ./tests/test_main
#     DEPENDS test_main
# )