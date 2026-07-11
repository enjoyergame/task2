register_all_tests_in_dir("${CMAKE_CURRENT_SOURCE_DIR}" hexdump_tests hexdump_lib)

target_include_directories(test_hexdump_tests_hexdump_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/src
)