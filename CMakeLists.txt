cmake_minimum_required(VERSION 3.20)

project(SearchEngine
    VERSION 1.0
    DESCRIPTION "A simple C++ search engine library"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(search SHARED
    src/cpp/search.cpp
    src/cpp/Tokenizer.cpp
)

target_include_directories(search
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Compiler warnings
target_compile_options(search PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# Build type specific settings
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build")

    target_compile_options(search PRIVATE
        -g
        -O0
    )

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Release build")

    target_compile_options(search PRIVATE
        -O3
        -DNDEBUG
    )

else()
    message(WARNING "Unknown build type: ${CMAKE_BUILD_TYPE}")
endif()
