#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__

#include <storage.h>
#include <threads.h>
struct transaction_context
{
    
    pthread_mutex_t *tsxid_lock;
    TSXID next;
};

struct transaction
{
    TSXID id;

    struct transaction_context ctx;
};

struct transaction*  begin_transaction(struct transaction_context *context);

#endif
