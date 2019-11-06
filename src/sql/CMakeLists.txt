cmake_minimum_required(VERSION 3.0.0)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(FLEX)
find_package(BISON 3.4.0)

FLEX_TARGET(btdb_scanner scanner.l  ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)
BISON_TARGET(btdb_parser
    parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
)
ADD_FLEX_BISON_DEPENDENCY(btdb_scanner btdb_parser)

add_library(btdb_sql_parser
    ${sources}
    ${FLEX_btdb_scanner_OUTPUTS}
    ${BISON_btdb_parser_OUTPUTS}
)

add_executable(parse_sql
    main.cpp
)
target_link_libraries(parse_sql
    btdb_sql_parser
)
target_compile_options(parse_sql
    PRIVATE
    -Wall
    -Wextra
)
