#ifndef __GPSQL_THREADS_H__
#define __GPSQL_THREADS_H__

#include <stdint.h>

typedef void* mutex;
// typedef void* event;
typedef void* gpsql_thread;
typedef void* (*gpsql_thread_func)(void*);

mutex create_mutex();
void destroy_mutex(mutex m);
void lock(mutex m);
void unlock(mutex m);

// event create_event();
// void event_set(event e);
// void event_wait(event e);
// void destroy_event(event e);

gpsql_thread create_thread(gpsql_thread_func func, void *args);
void join_thread(gpsql_thread *thread, uint32_t timeout);
void release_thread(gpsql_thread *thread);
void wait_till(uint32_t milliseconds);

#endif