#include <npsql.h>
#include <networking.h>

#include <stdio.h>

#define LISTENING_PORT 15151

void npsql_start(void)
{
    printf("Starting npsql\n");

    start_server(LISTENING_PORT);
}
