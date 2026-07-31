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

# ---------------------------------------
# Dependencies 
# ---------------------------------------

include(FetchContent)

FetchContent_Declare(
    curl
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG curl-8_20_0
)

set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(curl)

## --------------------------------------------------
# Components
# --------------------------------------------------

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

function(add_app NAME PATH)
  add_executable(${NAME} ${PATH})
  target_compile_options(${NAME} PRIVATE
      -Wall
      -Wextra
      -Wpedantic
  )
    
    # Build type specific settings
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${NAME} PRIVATE
            -g
            -O0
        )
        target_compile_definitions(${NAME} PRIVATE
            TEST
        )


    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${NAME} PRIVATE
            -O3
            -DNDEBUG
        )

    endif()

endfunction()

add_component(common)
add_component(crawler)
add_component(indexer)
#add_component(server)

target_link_libraries(indexer
    PUBLIC common
)

target_link_libraries(crawler
    PUBLIC curl
)


# ----------------------------------------
# Executables
# ----------------------------------------
add_app(crawler_app "${CMAKE_CURRENT_SOURCE_DIR}/apps/crawler_main.cpp")
target_link_libraries(crawler_app PRIVATE crawler common)

add_app(indexer_app "${CMAKE_CURRENT_SOURCE_DIR}/apps/indexer_main.cpp")
target_link_libraries(indexer_app PRIVATE indexer common)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Debug build")
    add_app(run_tests "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_main.cpp")
    target_link_libraries(run_tests PRIVATE crawler indexer common)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Release build")
else()
    message(WARNING "Unknown build type: ${CMAKE_BUILD_TYPE}")
endif()


#add_executable(search_server apps/server_main.cpp)
#target_link_libraries(search_server PRIVATE server)
