cmake_minimum_required(VERSION 3.15)
project(HexDump C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_TESTING "Build the testing suite" ON)

add_subdirectory(lib)
add_subdirectory(src)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()