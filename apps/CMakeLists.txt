
add_executable(parse_sql parse_sql.c)
target_link_libraries(parse_sql
    foo_lib
    #btdb_sql_parser
    #stb
)

