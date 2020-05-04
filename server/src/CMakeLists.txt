if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-pthread -Wall -Wextra -pedantic -Werror)
endif()

set (COMMON_SOURCE
    common.c
    npsql.c
    networking.c
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