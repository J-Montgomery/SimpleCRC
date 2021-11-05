
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

check_include_files("getopt.h" HAVE_GETOPT_H)
if(NOT HAVE_GETOPT_H)
    message(FATAL_ERROR "dependency getopt not found")
endif()

check_function_exists(strsep HAVE_STRSEP)
if(NOT HAVE_STRSEP)
    message(FATAL_ERROR "strsep not found")
endif()

add_executable(test_executable src/test.c)
target_link_libraries(test_executable ${LibName})
add_test(crc_large_test test_executable)


add_executable(test_bit_executable src/bit_test.c)
target_link_libraries(test_bit_executable ${LibName})
add_test(crc_bit_test test_bit_executable)

add_executable(test_db_executable src/db_test.c)
target_link_libraries(test_db_executable ${LibName})
add_test(crc_db_test test_db_executable)

set_tests_properties(crc_large_test crc_bit_test crc_db_test PROPERTIES DEPENDS ${CRC_DB_FILE})
