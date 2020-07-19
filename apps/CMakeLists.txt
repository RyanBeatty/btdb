
add_executable(parse_sql parse_sql.c)
target_link_libraries(parse_sql
    foo_lib
)

add_executable(btdb btdb.c)
target_link_libraries(btdb
    foo_lib
)

