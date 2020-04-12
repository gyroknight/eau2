target_include_directories(eau2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(eau2-registrar "")
target_include_directories(eau2-registrar PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_sources(eau2-registrar
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/registrar.cpp")
target_compile_options(eau2-registrar
    PUBLIC -Wall
)