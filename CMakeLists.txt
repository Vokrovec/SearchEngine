cmake_minimum_required(VERSION 3.20)

project(SearchEngine
    VERSION 1.0
    DESCRIPTION "A simple C++ search engine"
    LANGUAGES CXX
)

# Use modern C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

# Main executable
add_executable(search
    src/main.cpp
)
