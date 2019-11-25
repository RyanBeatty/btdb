cmake_minimum_required(VERSION 3.0.0)

add_executable(test_unity_exec test_unity.c)
target_link_libraries(test_unity_exec unity)
add_test(test_unity ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_unity_exec)
