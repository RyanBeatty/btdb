find_package(FLEX)
find_package(BISON 3.4.0)

FLEX_TARGET(btdb_scanner
    scanner.l
    # THIS IS SUPER IMPORTANT. ALLOWS OUT OF SOURCE BUILDS
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.c
)
BISON_TARGET(btdb_parser
    parser.y
    # THIS IS SUPER IMPORTANT. ALLOWS OUT OF SOURCE BUILDS
    ${CMAKE_CURRENT_BINARY_DIR}/parser.c
)
ADD_FLEX_BISON_DEPENDENCY(btdb_scanner btdb_parser)

add_library(foo_lib
    types.c
    storage.c
    utils.c
    node.c
    plan.c
    analyzer.c

    ${FLEX_btdb_scanner_OUTPUTS}
    ${BISON_btdb_parser_OUTPUTS}
    driver.c
)

# Add location for header files.
target_include_directories(foo_lib
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    # Flex and Bison files are generated here, so we need to add as include dirs
    # in order to see header files.
    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}"
)

# stb is used as a dependency, so link it.
target_link_libraries(foo_lib stb)

target_compile_options(foo_lib
    PUBLIC
    -Werror
    -Wall
    -Wextra
)


#[[target_link_libraries(btdb btdb_lib)
target_compile_options(btdb
    PRIVATE
    -Werror
    -Wall
    -Wextra
)]]
