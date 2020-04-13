add_executable(libsorer_test test.cpp)

target_include_directories(libsorer_test PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")

# target_link_libraries(libsorer_test sorer)
target_link_libraries(libsorer_test gtest_main)

# link data
add_custom_target(libsorer_test_link
    COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_SOURCE_DIR}/data.tar.gz"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Unpacking data.tar.gz to build dir"
    )

add_dependencies(libsorer_test libsorer_test_link)
