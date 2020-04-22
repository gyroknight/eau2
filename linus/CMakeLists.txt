project(linus VERSION 0.01
             DESCRIPTION "7 degrees of Linus"
             LANGUAGES CXX)

add_executable(linus "")
target_include_directories(demo PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_sources(linus
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(linus eau2 sorer)

target_compile_options(linus
    PUBLIC -Wall
)