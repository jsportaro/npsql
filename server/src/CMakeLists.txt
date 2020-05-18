option(WithBison "Generate the parser/lexer with Bison/Flex" OFF)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-pthread -Wall -Wextra -pedantic -Werror)
endif()

if(WithBison)
    find_package(BISON REQUIRED)
    find_package(FLEX  REQUIRED)

    BISON_TARGET(npsql_parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c)
    FLEX_TARGET(npsql_lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)
    
    ADD_FLEX_BISON_DEPENDENCY(npsql_lexer npsql_parser)

    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    SET (PARSER_SOURCE
        ${BISON_npsql_parser_OUTPUTS}
        ${FLEX_npsql_lexer_OUTPUTS}
        )

    message("Parser generated with BISON")
else()
    SET (PARSER_SOURCE
    )
    message("Parser generated with roll your own")
endif()

set (COMMON_SOURCE
    common.c
    npsql.c
    networking.c
    ${PARSER_SOURCE}
    )

if (WIN32)
    set (TARGET_SOURCE
        ${COMMON_SOURCE}
        )
endif()

if (UNIX)
    set (TARGET_SOURCE
        networking_linux.c
        threads_linux.c
        ${COMMON_SOURCE}
        )
endif()

add_library(backend ${TARGET_SOURCE})

add_executable(npsql main.c)


if (MSVC)
	target_link_libraries(npsql PRIVATE backend)
else()
	target_link_libraries(npsql PRIVATE backend PRIVATE Threads::Threads)
endif()