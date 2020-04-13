project(libeau2 VERSION 0.01
             DESCRIPTION "Distributed key-value store"
             LANGUAGES CXX)

add_library(eau2 "")

add_subdirectory(database)
add_subdirectory(network)
add_subdirectory(serial)
add_subdirectory(tests)

target_link_libraries(eau2 Threads::Threads)
target_include_directories(eau2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../libsorer/include")

target_compile_options(eau2
    PUBLIC -Wall
)
