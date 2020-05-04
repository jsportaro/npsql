#include <common.h>
#include <npsql.h>
#include <networking.h>

#include <stdio.h>

#define LISTENING_PORT 15151

const int i = 1;

void npsql_start(void)
{
    struct npsql_server server;
    struct session_manager session_manager;
    server.is_little_endian = ((*(char *)&i) == 1);

    fprintf(stdout, "Starting npsql\n");
    fprintf(stdout, "Currently running on a %s machine\n", server.is_little_endian == true ? "little endian" : "big endian");

    session_manager_init(&session_manager);
    server_start(LISTENING_PORT, &session_manager);
}
