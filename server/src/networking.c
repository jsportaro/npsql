#include <common.h>
#include <networking.h>
#include <threads.h>

#include <stdbool.h>
#include <stdio.h>

uint8_t welcome[5] = { (char)NQP_WELCOME,      0x02, 0x00, 0x00, 0x00};
uint8_t sorry[3]   = { (char)NQP_SORRY,        0x00, 0x00 };
uint8_t goodbye[3] = { (char)NQP_COMEBACKSOON, 0x00, 0x00 };

static int handle_hello(void *network_handle, uint8_t *session_id)
{
    uint8_t header[3];

    receive_buffer(network_handle, header, sizeof(header));

    if (header[0] != (uint8_t)NQP_HELL0)
    {
        return PROTOCOL_ERROR;
    }

    if (ptohs(&header[1]) != SESSION_ID_LENGTH)
    {
        return PROTOCOL_ERROR;
    }

    receive_buffer(network_handle, session_id, SESSION_ID_LENGTH);

    return PROTOCOL_OK;
}

static void handle_goodbye(struct session *session)
{
    send_buffer(session->network_handle, goodbye, sizeof(goodbye));
}

static void handle_query(struct session *session, size_t payload_size)
{
    uint8_t *query = malloc(payload_size + 1);

    receive_buffer(session->network_handle, query, payload_size);

    query[payload_size] = '\0';

    fprintf(stdout, "%s - sent\n", session->session_id);
}

static void return_session(struct session *session)
{
    join_thread(session->query_loop, 0);

    free(session->query_loop);

    lock(session->manager->lock);

    session->manager->next_open--;
    session->manager->open_sessions[session->manager->next_open] = session->id;
    session->network_handle = NULL;

    fprintf(stdout, "Session Id %d has been returned to the pool\n", session->manager->open_sessions[session->manager->next_open]);

    unlock(session->manager->lock);
}

void* query_loop(void* args)
{
    struct session *this = (struct session *)args;
    bool more_to_come = true;

    fprintf(stdout, "%s - is in it's query_loop\n", this->session_id);

    while (more_to_come)
    {
        uint8_t header[3];

        receive_buffer(this->network_handle, header, sizeof(header));

        switch ((enum NQP_MESSAGE)header[0])
        {
            case NQP_GOODBYE:
                handle_goodbye(this);
                close_connection(this->network_handle);
                more_to_come = false;
                break;
            case NQP_QUERY:
                handle_query(this, (size_t)ptohs(&header[1]));
            default:
                break;
        }
    }

    fprintf(stdout, "%s - has disconnected\n", this->session_id);

    return_session(this);

    return NULL;
}

void session_manager_init(struct session_manager *session_manager)
{
    htops(MAX_MESSAGE_SIZE, &welcome[3]);

    for (int i = 0; i < MAX_SESSION_COUNT; i++)
    {
        session_manager->open_sessions[i] = i;
        session_manager->sessions[i].id = i;
        session_manager->sessions[i].manager = session_manager;
    }

    session_manager->next_open = 0;
    session_manager->lock = create_mutex();
}

void handle_connection(struct session_manager *session_manager, void *network_handle)
{
    int free_slot = -1;
    uint8_t session_id[SESSION_ID_LENGTH];

    if (handle_hello(network_handle, session_id) != PROTOCOL_OK)
    {
        fprintf(stderr, "Hello message malformed\n");

        return;
    }

    lock(session_manager->lock);

    if (session_manager->next_open != MAX_SESSION_COUNT)
    {
        free_slot = session_manager->open_sessions[session_manager->next_open];
        session_manager->next_open++;
    }

    unlock(session_manager->lock);

    if (free_slot < 0)
    {
        fprintf(stdout, "Saying sorry\n");
        send_buffer(network_handle, sorry, sizeof(sorry));
    }
    else
    {
        struct session *session = &session_manager->sessions[free_slot];

        session->network_handle = network_handle;
        uuid_string(session_id, session->session_id);

        fprintf(stdout, "%s - Saying welcome\n", session->session_id);
        send_buffer(network_handle, welcome, sizeof(welcome));

        session->query_loop = create_thread(&query_loop, session);

        fprintf(stdout, "Session Id %d has been given from the pool\n", free_slot);
    }
}

void session_start(struct session *session)
{
    UNUSED(session);
}