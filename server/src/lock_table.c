#include <lock_table.h>
#include <storage.h>
#include <threads.h>
#include <vector.h>

#include <stdbool.h>
#include <stdint.h>

static struct db_lock * 
find_existing_lock(struct lock_table *lt, PNUM pnum);

static struct db_lock * 
add_rw_lock(struct lock_table *lt, PNUM pnum);

void 
create_lock_table(struct lock_table *lt)
{
    lt->m = create_mutex();
    lt->locks = NULL;
}

void free_lock_table(struct lock_table *lock_table)
{
    vector_free(lock_table->locks);
    destroy_mutex(lock_table->m);
}

void 
slock(struct lock_table *lt, PNUM pnum)
{
try_again:
    lock(lt->m);

    struct db_lock *l = find_existing_lock(lt, pnum);

    if (l == NULL)
    {
        l = add_rw_lock(lt, pnum);
    }

    if (l->read_count < 0)
    {
        unlock(lt->m);
        wait_till(100);
        goto try_again;
    }
    else
    {
        l->read_count++;
    }
    
    unlock(lt->m);

    return;
}

void 
xlock(struct lock_table *lt, PNUM pnum)
{
try_again:
    lock(lt->m);

    struct db_lock *l = find_existing_lock(lt, pnum);

    if (l == NULL)
    {
        l = add_rw_lock(lt, pnum);
    }

    if (l->read_count != 0)
    {
        unlock(lt->m);
        wait_till(100);
        goto try_again;
    }
    else
    {
        l->read_count--;
    }
    
    unlock(lt->m);
    return;
}

void 
release(struct lock_table *ls, PNUM pnum)
{
    lock(ls->m);

    struct db_lock *l = find_existing_lock(ls, pnum);

    if (l == NULL)
    {
        return;
    }

    if (l->read_count > 0)
    {
        l->read_count--;
    }
    else
    {
        l->read_count++;
        l->pnum = INVALID_PNUM;
    }
    
    unlock(ls->m);
}

static struct db_lock * 
find_existing_lock(struct lock_table *lt, PNUM pnum)
{
    for (size_t i = 0; lt->locks != NULL && i < vector_size(lt->locks); i++)
    {
        if (lt->locks[i].pnum == pnum)
        {
            return &lt->locks[i];
        }
    }
    
    return NULL;
}

static struct db_lock *
add_rw_lock(struct lock_table *lt, PNUM pnum)
{
    struct db_lock new_rw_lock = { 0 };

    new_rw_lock.pnum = pnum;
    new_rw_lock.read_count = 0;
            
    vector_push(lt->locks, new_rw_lock);
    
    size_t i = vector_size(lt->locks) - 1;
    return &lt->locks[i];
}
