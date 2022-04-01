cmake_minimum_required(VERSION 3.18) # For SOURCE_SUBDIR in FetchContent_Declare

project(lsh CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra)

option(LSH_BUILD_EXAMPLE "Builds the lsh example." ON)
option(LSH_BUILD_TESTS "Build the lsh tests." ON)


if (LSH_BUILD_EXAMPLE)
    include(example)
endif ()

if (LSH_BUILD_TESTS)
    include(tests)
endif ()

include(lsh)
