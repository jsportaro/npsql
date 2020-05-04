#ifndef __NETWORKING_H__
#define __NETWORKING_H__

#include <threads.h>

#include <stdint.h>
#include <stdio.h>

#define MAX_SESSION_COUNT 10
#define MAX_MESSAGE_SIZE  1000

#define SESSION_ID_LENGTH 16
#define UUID_LENGTH 37

#define PROTOCOL_ERROR -1
#define PROTOCOL_OK 1

enum NQP_MESSAGE
{
    NQP_NO,
    NQP_HELL0,
    NQP_WELCOME,
    NQP_SORRY,
    NQP_GOODBYE,
    NQP_COMEBACKSOON,
    NQP_QUERY,
    NQP_COLUMNDEFINITION,
    NQP_ROWSET,
    NQP_COMPLETED,
    NQP_READY
};

struct nqp_header
{
    union {
        enum NQP_MESSAGE tag;
        char bytes[sizeof(enum NQP_MESSAGE)];
    } type; 
};

struct session 
{
    uint32_t id;
    void *network_handle;

    gpsql_thread query_loop;
    struct session_manager *manager;
    char session_id[UUID_LENGTH];
};

struct session_manager
{
    int open_sessions[MAX_SESSION_COUNT];
    int next_open;
    struct session sessions[MAX_SESSION_COUNT];
    mutex lock;
};

void server_start(uint16_t port, struct session_manager *session_manager);
void session_manager_init(struct session_manager *session_manager);
void handle_connection(struct session_manager *session_manager, void *network_handle);
void close_connection(void *network_handle);

void send_buffer(void *network_handle, uint8_t *buffer, size_t size);
void receive_buffer(void *network_handle, uint8_t *buffer, size_t size);


#endif
