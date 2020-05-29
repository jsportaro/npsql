add_executable(parser_test parser_test.c)
add_executable(npsql_tests npsql_tests.c)
add_executable(buffer_manager_tests buffer_manager_tests.c)
add_executable(buffers_tests buffers_tests.c)

target_link_libraries(buffers_tests PRIVATE backend)
target_link_libraries(buffer_manager_tests PRIVATE backend)
target_link_libraries(npsql_tests PRIVATE backend)
target_link_libraries(parser_test PRIVATE backend)
