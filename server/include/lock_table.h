#ifndef __LOCK_TABLE_H__
#define __LOCK_TABLE_H__

#include <storage.h>
#include <threads.h>
#include <vector.h>

#include <stdbool.h>
#include <stdint.h>

struct db_lock
{
    int32_t read_count;
    PNUM pnum;
};

struct lock_table
{
    pthread_mutex_t *m;

    vector_type(struct db_lock) locks;
};

void create_lock_table(struct lock_table *lock_table);
void slock(struct lock_table *lock_table, PNUM page_number);
void xlock(struct lock_table* lock_table, PNUM page_number);
void release(struct lock_table *ls, PNUM pnum)
#endif
