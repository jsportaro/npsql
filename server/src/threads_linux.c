#include <threads.h>

#include <pthread.h> 
#include <stdlib.h> 
#include <unistd.h>

mutex create_mutex()
{
    pthread_mutex_t *lock = malloc(sizeof(pthread_mutex_t)); 

    pthread_mutex_init(lock, NULL);

    return lock;
}

void destroy_mutex(mutex m)
{
    free(m);
}

void lock(mutex m)
{
    pthread_mutex_lock(m);
}

void unlock(mutex m)
{
    pthread_mutex_unlock(m); 
}