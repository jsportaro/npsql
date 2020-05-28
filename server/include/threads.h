#ifndef __THREADS_H__
#define __THREADS_H__

#include <pthread.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

typedef void* mutex;
// typedef void* event;
typedef void* gpsql_thread;
typedef void* (*gpsql_thread_func)(void*);

pthread_mutex_t * create_mutex();
void destroy_mutex(pthread_mutex_t *m);
void lock(pthread_mutex_t *m);
void unlock(pthread_mutex_t *m);

// event create_event();
// void event_set(event e);
// void event_wait(event e);
// void destroy_event(event e);

gpsql_thread create_thread(gpsql_thread_func func, void *args);
void join_thread(gpsql_thread *thread, uint32_t timeout);
void release_thread(gpsql_thread *thread);
void wait_till(uint32_t milliseconds);

#endif
