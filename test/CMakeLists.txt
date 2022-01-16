
enable_testing()

function(CheckPy3Package PackageName)
    execute_process(
        COMMAND pip3 show ${PackageName} RESULT_VARIABLE EXIT_CODE OUTPUT_QUIET)

    if(NOT ${EXIT_CODE} EQUAL 0)
        message(FATAL_ERROR
            "Python3 package \"" ${PackageName} "\" is not installed")
    else()
        message(STATUS "Found Python3: " ${PackageName})
    endif()
endfunction()

function(AddRvMatchTest TargetName)
find_program(RV_MATCH NAMES rv-match)
if (RV_MATCH)
    get_target_property(TARGET_SOURCES ${TargetName} SOURCES)
    get_target_property(TARGET_LIBS ${TargetName} LINK_LIBRARIES)

    # Get the include dirs from each lib we have
    foreach(LIB ${TARGET_LIBS})
        get_target_property(LIB_INCLUDES ${LIB} INCLUDE_DIRECTORIES)
        list(APPEND TARGET_INCLUDES ${LIB_INCLUDES})
    endforeach()

    # We know all tests will link SimpleCrc, so just append those sources
    # automatically
    list(APPEND TARGET_SOURCES ${SimpleCrcSources})

    # Get the absolute path to all sources
    foreach(SRC ${TARGET_SOURCES})
        get_filename_component(FPATH ${SRC} ABSOLUTE)
        list(APPEND ABS_SOURCES ${FPATH})
    endforeach()

    add_test(NAME "rv-match-${TargetName}" COMMAND
        rv-match ${ABS_SOURCES} -I ${TARGET_INCLUDES}
    )
endif()
endfunction()

find_package(Python3 COMPONENTS Interpreter)
CheckPy3Package(pip)
CheckPy3Package(requests)
CheckPy3Package(beautifulsoup4)

set(CWD ${CMAKE_CURRENT_SOURCE_DIR})
set(CRC_DB_FILE ${CWD}/crc_db.txt)
set(CRC_DB_URL "https://reveng.sourceforge.io/crc-catalogue/all.htm")

add_custom_target(regen-test-db
        COMMAND ${Python3_EXECUTABLE} ${CWD}/test_init.py
        -u ${CRC_DB_URL} -o ${CRC_DB_FILE}
        BYPRODUCTS ${CRC_DB_FILE}
    )
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)


# Due to CMake issues, this ends up regenerating the catalog every time
# tests are run. Not sure how to work around this as the byproducts are
# correctly specified
add_dependencies(check regen-test-db)

check_include_files("getopt.h" HAVE_GETOPT_H)
if(NOT HAVE_GETOPT_H)
    message(FATAL_ERROR "dependency getopt not found")
endif()

check_function_exists(strsep HAVE_STRSEP)
if(NOT HAVE_STRSEP)
    message(FATAL_ERROR "strsep not found")
endif()

if(ENABLE_FUZZING)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O1 -fsanitize=fuzzer,address")
    add_executable(fuzz_test src/fuzzer_test.c)
    target_link_libraries(fuzz_test ${LibName})

    return() # Stop processing further tests
endif()

add_executable(crc_large_test src/basic_test.c)
target_link_libraries(crc_large_test ${LibName})
add_test(crc_large_test crc_large_test)

add_executable(crc_db_test src/db_test.c)
target_link_libraries(crc_db_test ${LibName})
add_test(NAME crc_db_test COMMAND
         crc_db_test ${CRC_DB_FILE})

set_tests_properties(crc_large_test crc_db_test PROPERTIES DEPENDS ${CRC_DB_FILE})

function(DefineExhaustiveTest TestName TestFile)
    add_executable(${TestName} ${TestFile})
    target_link_libraries(${TestName} ${LibName})
    add_test(${TestName} ${TestName})
endfunction()

# 8-15 bit tests
DefineExhaustiveTest(cdma2000_8_test src/exhaustive_cdma2000_8.c)
DefineExhaustiveTest(dect_12_test src/exhaustive_dect_12.c)

# 16-31 bit tests
DefineExhaustiveTest(kermit_test src/exhaustive_kermit.c)
DefineExhaustiveTest(ibm3740_test src/exhaustive_ibm3740.c)
DefineExhaustiveTest(lte_a_test src/exhaustive_lte_a.c)

# 32-63 bit tests
DefineExhaustiveTest(autosar32_test src/exhaustive_autosar32.c)
DefineExhaustiveTest(gsm_40_test src/exhaustive_gsm40.c)

# MISC exhaustive tests
DefineExhaustiveTest(go_iso_test src/exhaustive_go_iso.c)


# rv-match verification tests
AddRvMatchTest(crc_large_test)
AddRvMatchTest(crc_db_test)

AddRvMatchTest(cdma2000_8_test)
AddRvMatchTest(dect_12_test)

AddRvMatchTest(kermit_test)
AddRvMatchTest(ibm3740_test)
AddRvMatchTest(lte_a_test)

AddRvMatchTest(autosar32_test)
AddRvMatchTest(gsm_40_test)
AddRvMatchTest(go_iso_test)
