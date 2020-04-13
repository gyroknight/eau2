project(libsorer VERSION 0.01
    DESCRIPTION "Schema-on-read parsing library"
    LANGUAGES CXX)

# Disabled as real library because this is library is header-only
# add_library(sorer "")

add_subdirectory(tests)
