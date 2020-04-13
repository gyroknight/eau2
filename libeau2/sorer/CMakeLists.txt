target_include_directories(eau2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_sources(eau2
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/linereader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/object.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sorparser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/strslice.cpp")