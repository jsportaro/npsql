#include <common.h>
#include <threads.h>

struct gpsql_thread_linux
{
    pthread_t pthread;
};


pthread_mutex_t * 
create_mutex()
{
    pthread_mutex_t *lock = malloc(sizeof(pthread_mutex_t)); 

    pthread_mutex_init(lock, NULL);

    return lock;
}

void 
destroy_mutex(pthread_mutex_t *m)
{
    free(m);
}

void 
lock(pthread_mutex_t *m)
{
    pthread_mutex_lock(m);
}

void 
unlock(pthread_mutex_t *m)
{
    pthread_mutex_unlock(m); 
}

gpsql_thread create_thread(gpsql_thread_func func, void *args)
{
    struct gpsql_thread_linux *thread = malloc(sizeof(struct gpsql_thread_linux));

    int ret = pthread_create(&thread->pthread, NULL, func, args);

    if (ret != 0)
    {
        fprintf(stderr, "Failed to create thread\n");
    }

    return thread;
}

void join_thread(gpsql_thread *t, uint32_t timeout)
{
    UNUSED(timeout);
    
    struct gpsql_thread_linux *thread = (struct gpsql_thread_linux *)t;
    pthread_join(thread->pthread, NULL);
}

void release_thread(gpsql_thread *thread)
{
    free(thread);
}

void wait_till(uint32_t milliseconds)
{
    usleep(milliseconds * 1000);
}
