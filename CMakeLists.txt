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
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(search_engine
  src/Tokenizer.cpp
  #include/Tokenizer.hpp
)

target_include_directories(search_engine
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Main executable
add_executable(search
    src/main.cpp
)

target_link_libraries(search
    PRIVATE
        search_engine
)
