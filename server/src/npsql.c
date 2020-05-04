#include <common.h>
#include <npsql.h>
#include <networking.h>

#include <stdio.h>

#define LISTENING_PORT 15151

const int i = 1;

void npsql_start(void)
{
    struct session_manager session_manager;

    fprintf(stdout, "Starting npsql\n");
    
    session_manager_init(&session_manager);
    server_start(LISTENING_PORT, &session_manager);
}
