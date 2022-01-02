
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

add_executable(test_executable src/test.c)
target_link_libraries(test_executable ${LibName})
add_test(crc_large_test test_executable)

add_executable(test_db_executable src/db_test.c)
target_link_libraries(test_db_executable ${LibName})
add_test(NAME crc_db_test COMMAND
         test_db_executable ${CRC_DB_FILE})

set_tests_properties(crc_large_test crc_db_test PROPERTIES DEPENDS ${CRC_DB_FILE})

function(DefineExhaustiveTest TestName TestFile)
    add_executable(${TestName} ${TestFile})
    target_link_libraries(${TestName} ${LibName})
    add_test(${TestName} ${TestName})
endfunction()

# 8-15 bit tests
DefineExhaustiveTest(cdma2000_8_test src/exhaustive_cdma2000_8.c)

# 16-31 bit tests
DefineExhaustiveTest(kermit_test src/exhaustive_kermit.c)
DefineExhaustiveTest(ibm3740_test src/exhaustive_ibm3740.c)

# 32-63 bit tests
DefineExhaustiveTest(autosar32_test src/exhaustive_autosar32.c)
DefineExhaustiveTest(gsm_40_test src/exhaustive_gsm40.c)

# MISC exhaustive tests
DefineExhaustiveTest(go_iso_test src/exhaustive_go_iso.c)
