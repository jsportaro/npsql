#include <common.h>
#include <threads.h>

#include <assert.h>

struct gpsql_thread_linux
{
    pthread_t pthread;
};


pthread_mutex_t * 
create_mutex()
{
    pthread_mutex_t *lock = malloc(sizeof(pthread_mutex_t)); 

    assert(lock != NULL);

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

struct event *
create_event()
{
    struct event *event = malloc(sizeof(struct event));

    assert(event != NULL);

    pthread_mutex_init(&event->mutex, NULL);
    pthread_cond_init(&event->condition, NULL);
    event->flag = 0;

    return event;
}

void 
event_set(struct event *e)
{
    pthread_mutex_lock(&e->mutex);
    e->flag = 1;
    pthread_cond_signal(&e->condition);
    pthread_mutex_unlock(&e->mutex);
}

void event_wait(struct event *e)
{
    pthread_mutex_lock(&e->mutex);

    while (!e->flag)
        pthread_cond_wait(&e->condition, &e->mutex);

    e->flag = 0;

    pthread_mutex_unlock(&e->mutex);
}

void destroy_event(struct event *e)
{
    free(e);
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
