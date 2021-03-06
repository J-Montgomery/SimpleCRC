file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}
        "*.h"
        "*.hpp"
        "*.c"
        "*.cpp")

find_program(CLANG_TIDY NAMES clang-tidy)
if (CLANG_TIDY)
    add_custom_target(
            tidy
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND ${CLANG_TIDY}
            ${SOURCE_FILES}
            --
            -std=c${CMAKE_C_STANDARD}
    )
endif ()


find_program(CLANG_FORMAT NAMES clang-format)
if (CLANG_FORMAT)
    add_custom_target(
            format
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND ${CLANG_FORMAT}
            -i ${SOURCE_FILES} ${TEST_SOURCE_FILES}
    )
endif ()


option(ASAN "Enable AddressSanitizer" OFF)
if (ASAN)
    message(STATUS "AddressSanitizer enabled for debug build")
    set(CMAKE_C_FLAGS_DEBUG
        "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
    set (CMAKE_LINKER_FLAGS_DEBUG
        "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif ()

option(UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
if (UBSAN)
    message(STATUS "UndefinedBehaviorSanitizer enabled for debug build")
    set(CMAKE_C_FLAGS_DEBUG
        "${CMAKE_C_FLAGS_DEBUG} -fsanitize=undefined")
    set (CMAKE_LINKER_FLAGS_DEBUG
        "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=undefined")
endif ()

option(MSAN "Enable MemorySanitizer" OFF)
if (MSAN)
    message(STATUS "MemorySanitizer enabled for debug build")
    set(CMAKE_C_FLAGS_DEBUG
        "${CMAKE_C_FLAGS_DEBUG} -fsanitize=memory")
    set (CMAKE_LINKER_FLAGS_DEBUG
        "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=memory")
endif ()