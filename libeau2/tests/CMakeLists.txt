add_executable(libeau2_test test.cpp)
target_include_directories(libeau2_test PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(libeau2_test eau2 gtest_main)