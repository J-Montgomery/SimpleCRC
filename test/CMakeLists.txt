
enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

add_executable(test_executable src/test.c)
target_link_libraries(test_executable ${LibName})
add_test(crc_large_test test_executable)

add_executable(test_bit_executable src/bit_test.c)
target_link_libraries(test_bit_executable ${LibName})
add_test(crc_bit_test test_bit_executable)