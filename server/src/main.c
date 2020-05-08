#include <common.h>
#include <npsql.h>
#include <networking.h>

#include <stdio.h>

void npsql_start(int argc, char *argv[]);

int main (int argc, char *argv[])
{
    npsql_start(argc, argv);
}

void npsql_start(int argc, char *argv[])
{
    UNUSED(argc);

    char *data_file = argv[1];
    char *log_file  = argv[2];

    struct query_engine query_engine; 
    struct session_manager session_manager;

    fprintf(stdout, "Starting npsql\n");
    
    query_engine_init(data_file, log_file, &query_engine);
    session_manager_init(&session_manager, &query_engine);
    server_start(LISTENING_PORT, &session_manager);
}