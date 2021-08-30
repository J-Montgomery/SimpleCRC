cmake_minimum_required(VERSION 3.0)
project(simplecrc VERSION 1.0.0 LANGUAGES C
        DESCRIPTION "Simple CRC library")

set(CMAKE_C_STANDARD 11)

include(CheckIncludeFiles)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

enable_testing()

add_library(simplecrc src/main.c src/crc_bit.c)

include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

check_include_files("getopt.h" HAVE_GETOPT_H)

if(NOT HAVE_GETOPT_H)
    message(FATAL_ERROR "dependency getopt not found")
endif()
