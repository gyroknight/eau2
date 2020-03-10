cmake_minimum_required(VERSION 3.2.2)

project(eau2)

add_subdirectory(src)
add_subdirectory(tests)

# PHONY target to run tests
add_custom_target(test
    COMMAND ./tests/test_main
    DEPENDS test_main
    COMMENT 
)

