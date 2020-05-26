add_executable(parser_test parser_test.c)
add_executable(npsql_tests npsql_tests.c)

target_link_libraries(npsql_tests PRIVATE backend)
target_link_libraries(parser_test PRIVATE backend)
