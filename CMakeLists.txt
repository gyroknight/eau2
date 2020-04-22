cmake_minimum_required(VERSION 3.2.2)

project(eau2 VERSION 0.01
             DESCRIPTION "Distributed key-value store"
             LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

find_package(Threads REQUIRED)

# Tool for checking for redundant includes
find_program(iwyu_path NAMES include-what-you-use iwyu)
if((NOT iwyu_path) AND (DEFINED CHECK_INCLUDES))
  message(FATAL_ERROR "Could not find the program include-what-you-use")
endif()

add_subdirectory(test_common)
add_subdirectory(libeau2)
add_subdirectory(libsorer)
add_subdirectory(demo)

# PHONY target to run all tests TODO(mike) make this smarter
add_custom_target(test
    DEPENDS test1 test2
)

# test1
add_custom_target(test1
    COMMAND "$<TARGET_FILE:libeau2_test>"
    DEPENDS libeau2_test
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:libeau2_test>"
)

# test2
add_custom_target(test2
    COMMAND "$<TARGET_FILE:libsorer_test>"
    DEPENDS libsorer_test
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:libsorer_test>"
)

# PHONY target to run valgrind
add_custom_target(valgrind
    COMMAND valgrind --leak-check=full --tool=memcheck "$<TARGET_FILE:libeau2_test>"
    DEPENDS libeau2_test
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:libeau2_test>"
)
