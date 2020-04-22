project(demo VERSION 0.01
             DESCRIPTION "M1 demo"
             LANGUAGES CXX)

add_executable(demo "")
target_include_directories(demo PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_sources(demo
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/demo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/OptParser.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/application.cpp")
target_link_libraries(demo eau2)

target_compile_options(demo
    PUBLIC -Wall
)