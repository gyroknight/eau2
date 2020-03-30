project(libeau2 VERSION 0.01
             DESCRIPTION "Distributed key-value store"
             LANGUAGES CXX)

add_subdirectory(tests)

add_library(eau2 "")
target_include_directories(eau2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_sources(eau2
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dataframe.cpp"
    # "${CMAKE_CURRENT_SOURCE_DIR}/src/kvstore.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/payload.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/row.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/schema.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/serializer.cpp")

target_compile_options(eau2
    PUBLIC -Wall
)
