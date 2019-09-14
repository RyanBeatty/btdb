cmake_minimum_required(VERSION 3.0.0)

add_subdirectory(sql)

add_executable(btdb main.cpp)
target_compile_options(btdb
    PRIVATE
    -Werror
    -Wall
    -Wextra
)
