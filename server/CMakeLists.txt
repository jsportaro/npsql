if (UNIX)
	set(THREADS_PREFER_PTHREAD_FLAG ON) 
	set(CMAKE_THREAD_PREFER_PTHREAD ON)
	find_package(Threads REQUIRED)
endif()

include_directories("include")

option(WithBison "Generate the parser/lexer with Bison/Flex" ON)

if(WithBison)
	ADD_DEFINITIONS(-DBISONED)
	include_directories(${CMAKE_BINARY_DIR}/codegen)
endif()

add_subdirectory(src)
add_subdirectory(tests)