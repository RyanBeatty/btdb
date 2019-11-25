cmake_minimum_required(VERSION 3.0.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(sql)

add_library(btdb_lib STATIC
    utils.c
    analyzer.cpp
    storage.cpp
    types.c
)
target_include_directories(btdb_lib PUBLIC .)

add_executable(btdb
    main.cpp
)
target_link_libraries(btdb btdb_sql_parser btdb_lib)
target_compile_options(btdb
    PRIVATE
    #-Werror
    -Wall
    -Wextra
)
