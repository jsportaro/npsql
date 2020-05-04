#ifndef __NPSQL_H__
#define __NPSQL_H__

#include <stdbool.h>

struct npsql_server
{
    bool is_little_endian;
};

void npsql_start(void);

#endif
