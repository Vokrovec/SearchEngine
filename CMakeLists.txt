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

function(add_component NAME)
    file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/${NAME}/*.cpp"
    )
    file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/${NAME}/*.hpp"
    )

    add_library(${NAME} 
      ${SOURCES}
      ${HEADERS}
    )

    target_include_directories(${NAME}
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )
endfunction()

add_component(common)
#add_component(crawler)
add_component(indexer)
#add_component(server)

target_link_libraries(indexer
    PUBLIC common
)

#add_executable(crawler_app apps/crawler_main.cpp)
#target_link_libraries(crawler_app PRIVATE crawler)

add_executable(indexer_app apps/indexer_main.cpp)
target_link_libraries(indexer_app PRIVATE indexer)

#add_executable(search_server apps/server_main.cpp)
#target_link_libraries(search_server PRIVATE server)

# Compiler warnings
target_compile_options(indexer_app PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# Build type specific settings
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build")

    target_compile_options(indexer_app PRIVATE
        -g
        -O0
    )

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Release build")

    target_compile_options(indexer_app PRIVATE
        -O3
        -DNDEBUG
    )

else()
    message(WARNING "Unknown build type: ${CMAKE_BUILD_TYPE}")
endif()

