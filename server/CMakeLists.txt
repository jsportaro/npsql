if (UNIX)
	set(THREADS_PREFER_PTHREAD_FLAG ON) 
	set(CMAKE_THREAD_PREFER_PTHREAD ON)
	find_package(Threads REQUIRED)
endif()

include_directories("include")
include_directories(${CMAKE_BINARY_DIR}/codegen)

add_subdirectory(src)
add_subdirectory(tests)