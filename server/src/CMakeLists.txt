
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-pthread -Wall -Wextra -pedantic -Werror)
endif()

if(WithBison)
    find_package(BISON REQUIRED)
    find_package(FLEX  REQUIRED)

    BISON_TARGET(
        npsql_parser parser.y 
        ${CMAKE_BINARY_DIR}/codegen/parser.gen.c
        COMPILE_FLAGS -Wconflicts-sr)
    
    FLEX_TARGET(
        npsql_lexer lexer.l 
        ${CMAKE_BINARY_DIR}/codegen/lexer.gen.c COMPILE_FLAGS 
        --header-file=${CMAKE_BINARY_DIR}/codegen/lexer.gen.h COMPILE_FLAGS)
    
    ADD_FLEX_BISON_DEPENDENCY(npsql_lexer npsql_parser)

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/codegen)

    SET (PARSER_SOURCE
        ${BISON_npsql_parser_OUTPUTS}
        ${FLEX_npsql_lexer_OUTPUTS}
        parser_bison.c
        )

    message("** Parser generated with BISON")
else()
    SET (PARSER_SOURCE
    )
    message("** Parser generated with roll your own")
endif()

set (COMMON_SOURCE
    buffers.c
    buffer_manager.c
    common.c
    create_table.c
    data_file.c
    data_page.c
    file.c
    heap_table.c
    insert.c
    lock_table.c
    log_file.c
    nqp.c
    npsql.c
    plans.c
    scans.c
    planner.c
    storage.c
    sql.c 
    syscat.c
    threads.c
    transaction.c
    value.c
    wal.c
    ${PARSER_SOURCE}
    )

if (UNIX)
    set (TARGET_SOURCE
        networking_linux.c
        ${COMMON_SOURCE}
        )
endif()

add_library(backend ${TARGET_SOURCE})
target_link_libraries(backend PRIVATE Threads::Threads )

add_executable(npsql main.c )
target_link_libraries(npsql PRIVATE backend)
