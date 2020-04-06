target_include_directories(eau2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_sources(eau2
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/get.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/kill.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/message.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/payload.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/put.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/reply.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/serializer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/waitandget.cpp")