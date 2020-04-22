target_include_directories(eau2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_sources(eau2
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/kvnetTcp.cpp")

add_executable(eau2-registrar "")
target_include_directories(eau2-registrar PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(eau2-registrar PUBLIC eau2)

target_sources(eau2-registrar
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/registrar.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/kvnetTcp.cpp")
target_compile_options(eau2-registrar
    PUBLIC -Wall
)
