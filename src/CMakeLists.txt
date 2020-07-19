#cmake_minimum_required(VERSION 3.0.0)

find_package(FLEX)
find_package(BISON 3.4.0)

FLEX_TARGET(btdb_scanner
    scanner.l 
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.c
)
BISON_TARGET(btdb_parser
    parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.c
)
ADD_FLEX_BISON_DEPENDENCY(btdb_scanner btdb_parser)

message("foo bar ${BISON_btdb_parser_OUTPUTS}")
add_library(foo_lib
    foo.c
    types.c
    storage.c
    utils.c
    node.c

    ${FLEX_btdb_scanner_OUTPUTS}
    ${BISON_btdb_parser_OUTPUTS}
    driver.c

    #node.c
    #analyzer.c
    #plan.c
)
target_include_directories(foo_lib
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    # Flex and Bison files are generated here, so we need to add as include dirs
    # in order to see header files.
    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}"
)
target_link_libraries(foo_lib stb)






#[[include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(sql)

add_library(btdb_lib STATIC
    utils.c
    analyzer.c
    storage.c
    types.c
    plan.c
)
target_include_directories(btdb_lib PUBLIC .)
target_link_libraries(btdb_lib stb btdb_sql_parser)

add_executable(btdb
    main.c
)
target_link_libraries(btdb btdb_lib)
target_compile_options(btdb
    PRIVATE
    -Werror
    -Wall
    -Wextra
)]]
