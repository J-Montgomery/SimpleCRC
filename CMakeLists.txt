cmake_minimum_required(VERSION 3.10)
project(simplecrc VERSION 2.0.0 LANGUAGES C
        DESCRIPTION "Simple CRC library")

set(CMAKE_C_STANDARD 11)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(GNUInstallDirs)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(SimpleCrcSources
    ${PROJECT_SOURCE_DIR}/src/core.c
    ${PROJECT_SOURCE_DIR}/src/crc_utility.c)

set(SimpleCrcHeaders
    ${PROJECT_SOURCE_DIR}/include/simplecrc.h
    ${PROJECT_SOURCE_DIR}/include/internal/crc_utility.h
    ${PROJECT_SOURCE_DIR}/include/simplecrc/reference.h)

set(LibName simplecrc)
set(IncludeDir ${PROJECT_SOURCE_DIR}/include)

if(NOT ENABLE_PEDANTIC_OPTS)
    option(ENABLE_PEDANTIC_OPTS "Enable pedantic compiler flags" ON)
endif()

if(ENABLE_PEDANTIC_OPTS)
add_compile_options(-Wall -Werror -Wextra -Wno-unused-parameter -pedantic)
endif()

add_library(${LibName} ${SimpleCrcSources})
target_include_directories(${LibName} PUBLIC ${IncludeDir})

include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" )
option(ENABLE_TESTS "Enable Tests" ON)
else()
option(ENABLE_TESTS "Enable Tests" OFF)
endif()

if(ENABLE_TESTS)
enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/test)
endif() # ENABLE_TESTS

install(TARGETS simplecrc DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${SimpleCRCHeaders} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})