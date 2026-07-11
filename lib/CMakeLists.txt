
add_library(hexdump_lib
    src/hexdump.c
)

target_include_directories(hexdump_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
