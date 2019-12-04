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

#FLEX_TARGET(btdb_scanner2 scanner2.l  ${CMAKE_CURRENT_BINARY_DIR}/scanner2.c)
#BISON_TARGET(btdb_parser2
#    parser2.y
#    ${CMAKE_CURRENT_BINARY_DIR}/parser2.c
#)
#ADD_FLEX_BISON_DEPENDENCY(btdb_scanner2 btdb_parser2)
#
#add_library(btdb_sql_parser2
#    ${sources}
#    ${FLEX_btdb_scanner2_OUTPUTS}
#    ${BISON_btdb_parser2_OUTPUTS}
#    "driver.c"
#    "../node.c"
#    "../node.h"
#    "../utils.c"
#)
#
#add_executable(parse_sql2
#    main2.c
#)
#target_link_libraries(parse_sql2 btdb_sql_parser2)

add_library(btdb_sql_parser
    ${sources}
    ${FLEX_btdb_scanner_OUTPUTS}
    ${BISON_btdb_parser_OUTPUTS}
    driver.c
    "../node.c"
    "../utils.c"
)

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
