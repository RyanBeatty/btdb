cmake_minimum_required(VERSION 3.0.0)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(FLEX)
find_package(BISON 3.4.0)

FLEX_TARGET(btdb_scanner scanner.l  ${CMAKE_CURRENT_BINARY_DIR}/scanner.c)
BISON_TARGET(btdb_parser
    parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.c
)
ADD_FLEX_BISON_DEPENDENCY(btdb_scanner btdb_parser)

add_library(btdb_sql_parser
    ${sources}
    ${FLEX_btdb_scanner_OUTPUTS}
    ${BISON_btdb_parser_OUTPUTS}
    driver.c
    "../node.c"
    "../utils.c"
)
target_include_directories(btdb_sql_parser PUBLIC src/sql)
target_link_libraries(btdb_sql_parser stb)

add_executable(parse_sql
    main.c
)
target_link_libraries(parse_sql
    btdb_sql_parser
)
target_compile_options(parse_sql
    PRIVATE
    -Wall
    -Wextra
)
