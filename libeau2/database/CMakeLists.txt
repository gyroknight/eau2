target_include_directories(eau2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_sources(eau2
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dataframe.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/key.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/kvstore.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/row.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/schema.cpp")