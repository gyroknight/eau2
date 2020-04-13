project(libsorer VERSION 0.01
    DESCRIPTION "Schema-on-read parsing library"
    LANGUAGES CXX)

add_library(sorer "")

target_include_directories(sorer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_sources(sorer
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/column.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/object.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser.cpp")

add_subdirectory(tests)
