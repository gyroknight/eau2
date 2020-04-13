add_executable(libeau2_test test.cpp)
target_include_directories(libeau2_test PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../database/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../network/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../serial/include"
    "$<TARGET_FILE_DIR:sorer>/include")
    
target_link_libraries(libeau2_test eau2 gtest_main)

# link data
add_custom_target(libeau2_test_link
    COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_SOURCE_DIR}/data_ints.txt.tar.gz"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Unpacking data_ints.txt.tar.gz"
    )
add_dependencies(libeau2_test libeau2_test_link)
