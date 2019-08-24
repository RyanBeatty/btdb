cmake_minimum_required(VERSION 3.0.0)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(FLEX)

FLEX_TARGET(btdb_scanner scanner.l  ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)

add_executable(scan_sql
    ${FLEX_btdb_scanner_OUTPUTS}
)
target_link_libraries(scan_sql
    ${FLEX_LIBRARIES}
)
target_compile_options(scan_sql
    PRIVATE
    -Wall
    -Wextra
)

