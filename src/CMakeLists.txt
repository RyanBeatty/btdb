cmake_minimum_required(VERSION 3.0.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(sql)

add_library(btdb_lib STATIC
    utils.c
    analyzer.cpp
    storage.c
    types.c
)
target_include_directories(btdb_lib PUBLIC .)
target_link_libraries(btdb_lib stb btdb_sql_parser)

add_executable(btdb
    main.cpp
)
target_link_libraries(btdb btdb_lib)
target_compile_options(btdb
    PRIVATE
    #-Werror
    -Wall
    -Wextra
)
