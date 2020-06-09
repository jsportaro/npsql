function (ADD_BACKEND_TEST test_file test_name)

ADD_EXECUTABLE(${test_file} "${test_file}.c")
TARGET_LINK_LIBRARIES(${test_file} backend Threads::Threads)	
ADD_TEST(${test_name} ${test_file})

endfunction()

include_directories(.)

ADD_BACKEND_TEST(parser_tests parser)
ADD_BACKEND_TEST(log_file_tests log_file)
ADD_BACKEND_TEST(data_file_tests data_file)
ADD_BACKEND_TEST(buffer_manager_tests buffer_manager)
ADD_BACKEND_TEST(wal_tests wal)
ADD_BACKEND_TEST(transaction_tests transaction)
ADD_BACKEND_TEST(heap_tests heap)
ADD_BACKEND_TEST(create_table_tests create_table)
ADD_BACKEND_TEST(npsql_tests npsql_nonet)
ADD_BACKEND_TEST(insert_tests insert)