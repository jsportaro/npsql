#ifndef __NQP_H__
#define __NQP_H__

#include <defaults.h>
#include <npsql.h>
#include <threads.h>

#include <stdint.h>

#define SESSION_ID_LENGTH 16
#define UUID_LENGTH 37

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
         // iOS Simulator
    #elif TARGET_OS_IPHONE
        // iOS device
    #elif TARGET_OS_MAC
        // Other kinds of Mac OS
    #else
    #   error "Unknown Apple platform"
    #endif
#elif __linux__
    typedef int SOCKET;
#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif

enum NQP_MESSAGE
{
    NQP_NO,
    NQP_HELL0,
    NQP_WELCOME,
    NQP_SORRY,
    NQP_GOODBYE,
    NQP_COMEBACKSOON,
    NQP_QUERY,
    NQP_COLUMNDEFINITION,
    NQP_ROWSET,
    NQP_COMPLETED,
    NQP_READY
};

struct session 
{
    uint32_t id;
    gpsql_thread query_loop;
    char session_id[UUID_LENGTH];
    struct session_manager *manager;
    SOCKET socket;
};

struct session_manager
{
    int open_sessions[MAX_SESSION_COUNT];
    int next_open;
    struct query_engine *engine;
    struct session sessions[MAX_SESSION_COUNT];
    mutex lock;
};

void session_manager_init(struct session_manager *session_manager, struct query_engine *query_engine);
void handle_connection(struct session_manager *session_manager, SOCKET socket);

#endif
