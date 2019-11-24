cmake_minimum_required(VERSION 3.0.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(sql)

add_executable(btdb
    main.cpp
    utils.c
    analyzer.cpp
    storage.cpp
    types.c
)
target_link_libraries(btdb btdb_sql_parser)
target_compile_options(btdb
    PRIVATE
    #-Werror
    -Wall
    -Wextra
)
