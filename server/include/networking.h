#ifndef __NETWORKING_H__
#define __NETWORKING_H__

#include <threads.h>

#include <stdint.h>
#include <stdio.h>

#define MAX_SESSION_COUNT 10
#define MAX_MESSAGE_SIZE  1000
enum NQP_MESSAGE
{
    NQP_NO,
    NQP_HELL0,
    NQP_WELCOME,
    NQP_SORRY,
    NQP_GOODBYE,
    NQP_COMEBACKSOON
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
void session_manager_get_free(struct session_manager *session_manager, void *network_handle);

void send_buffer(void *network_handle, char *buffer, size_t size);
void receive_buffer(void *network_handle, char *buffer, size_t size);

#define UNUSED(expr) do { (void)(expr); } while (0)

#endif
