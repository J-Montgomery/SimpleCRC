cmake_minimum_required(VERSION 3.12)
project(simplecrc VERSION 1.1.0 LANGUAGES C
        DESCRIPTION "Simple CRC library")

set(CMAKE_C_STANDARD 11)

include(CheckIncludeFiles)
include(CheckFunctionExists)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(LibName simplecrc)
set(IncludeDir ${CMAKE_SOURCE_DIR}/include)

add_library(${LibName} src/main.c src/crc_bit.c src/crc_utility.c)
target_include_directories(${LibName} PUBLIC ${IncludeDir})

include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" )
option(ENABLE_TESTS "Enable Tests" ON)
else()
option(ENABLE_TESTS "Enable Tests" OFF)
endif()

if(ENABLE_TESTS)
add_subdirectory(${CMAKE_SOURCE_DIR}/test)

endif() # ENABLE_TESTS
