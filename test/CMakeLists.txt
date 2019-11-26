cmake_minimum_required(VERSION 3.0.0)

add_executable(test_collections_exec test_collections.c)
target_link_libraries(test_collections_exec unity btdb_lib)
add_test(test_collections ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_collections_exec)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
    DEPENDS test_collections_exec)
