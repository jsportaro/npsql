#ifndef __QUERY_CONTEXT_H__
#define __QUERY_CONTEXT_H__

#include <syscat.h>
#include <transaction.h>

struct query_ctx
{
    struct syscat *cat;
    struct transaction *tsx;
};

#endif
