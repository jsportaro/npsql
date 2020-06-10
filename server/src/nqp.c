#include <buffers.h>
#include <defaults.h>
#include <nqp.h>
#include <plans.h>
#include <scans.h>

#include <networking.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define HEADER_LENGTH 3
#define HEADER_TYPE_OFFSET 0
#define HEADER_SIZE_OFFSET 1

#define COMPLETED_MIN_LENGTH     6
#define COMPLETED_MIN_PAYLOAD    3
#define COMPLETED_RESULT_OFFSET  (HEADER_LENGTH)
#define COMPLETED_SIZE_OFFSET    (HEADER_LENGTH + 1)
#define COMPLETED_MESSAGE_OFFSET (HEADER_LENGTH + 3)

#define COLUMN_MIN_LENGTH 5

#define DEFAULT_ROW_BYTES 512

enum completed_status
{
    COMPLETED_UNKNONW,
    COMPLETED_SUCCESS,
    COMPLETED_FAILURE
};

#define PROTOCOL_ERROR -1
#define PROTOCOL_OK 1

uint8_t sorry  [3] = { (uint8_t)NQP_SORRY,        0x00, 0x00 };
uint8_t welcome[5] = { (uint8_t)NQP_WELCOME,      0x02, 0x00, 0x00, 0x00};
uint8_t goodbye[3] = { (uint8_t)NQP_COMEBACKSOON, 0x00, 0x00 };

static void* query_loop(void* args);
static int handle_hello(SOCKET socket, uint8_t *session_id);
static void handle_goodbye(struct session *session);
static void say_completed(struct session *session, enum completed_status status, vector_type(char) message);
static void say_columns(struct session *session, struct query_results *results);
static vector_type(uint8_t) say_rowset(struct session *session, vector_type(uint8_t) rowset_bytes);
static void handle_query(struct session *session, size_t payload_size);
static void return_session(struct session *session);

void session_manager_init(struct session_manager *session_manager, struct query_engine *query_engine)
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
    session_manager->engine = query_engine;
}

void handle_connection(struct session_manager *session_manager, SOCKET socket)
{
    int free_slot = -1;
    uint8_t session_id[SESSION_ID_LENGTH];

    if (handle_hello(socket, session_id) != PROTOCOL_OK)
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
        send_buffer(socket, sorry, sizeof(sorry));

    }
    else
    {
        struct session *session = &session_manager->sessions[free_slot];

        session->socket = socket;
        uuid_string(session_id, session->session_id);

        fprintf(stdout, "%s - Saying welcome\n", session->session_id);
        send_buffer(socket, welcome, sizeof(welcome));

        session->query_loop = create_thread(&query_loop, session);

        fprintf(stdout, "Session Id %d has been given from the pool\n", free_slot);
    }
}

static void* query_loop(void* args)
{
    struct session *this = (struct session *)args;
    bool more_to_come = true;

    fprintf(stdout, "%s - is in it's query_loop\n", this->session_id);

    while (more_to_come)
    {
        uint8_t header[3];

        receive_buffer(this->socket, header, sizeof(header));

        switch ((enum NQP_MESSAGE)header[0])
        {
            case NQP_GOODBYE:
                handle_goodbye(this);
                close_connection(this->socket);
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

static int handle_hello(SOCKET socket, uint8_t *session_id)
{
    uint8_t header[3];


    receive_buffer(socket, header, sizeof(header));

    if (header[0] != (uint8_t)NQP_HELL0)
    {
        return PROTOCOL_ERROR;
    }

    if (ptohs(&header[1]) != SESSION_ID_LENGTH)
    {
        return PROTOCOL_ERROR;
    }

    receive_buffer(socket, session_id, SESSION_ID_LENGTH);

    return PROTOCOL_OK;
}

static void handle_goodbye(struct session *session)
{
    send_buffer(session->socket, goodbye, sizeof(goodbye));
}

static void say_completed(struct session *session, enum completed_status status, vector_type(char) message)
{
    size_t msg_size = vector_size(message);
    size_t msg_length = COMPLETED_MIN_LENGTH + msg_size;
    uint8_t *completed = malloc(msg_length);

    // Header
    completed[HEADER_TYPE_OFFSET] = (uint8_t)NQP_COMPLETED;
    htops(COMPLETED_MIN_PAYLOAD + msg_size, &completed[HEADER_SIZE_OFFSET]);

    // Payload
    completed[COMPLETED_RESULT_OFFSET] = (uint8_t)status;
    htops((int16_t)msg_size, &completed[COMPLETED_SIZE_OFFSET]);
    memcpy(&completed[COMPLETED_MESSAGE_OFFSET], message, msg_size);

    send_buffer(session->socket, completed, msg_length);

    free(completed);
}

static void say_columns(struct session *session, struct query_results *results)
{  
    UNUSED(results);

    vector_type(uint8_t) column_bytes = NULL;

    // Header 
    vector_push(column_bytes, (uint8_t)NQP_COLUMNDEFINITION);
    vector_push(column_bytes, 0x00);
    vector_push(column_bytes, 0x00);

    // Payload
    vector_type(struct column) columns = NULL;
    
    if (results->current_plan->type == PROJECT_PLAN)
    {
        columns = ((struct plan *)results->current_plan)->column_list;
    
        for (size_t i = 0; i < vector_size(columns); i++)
        {
            struct column col = columns[i];
            long start_size = vector_size(column_bytes);
            uint16_t length = COLUMN_MIN_LENGTH + strlen(col.name);
            
            vector_grow(column_bytes, length  + start_size);

            uint8_t *column_start = &column_bytes[start_size];

            column_start[0] = (uint8_t)col.type;                         //  Column Type
            htops((uint16_t)col.size, &column_start[1]);                 //  Column Type Size (# bytes)
            htops((uint16_t)vector_size(col.name), &column_start[3]);          //  Name Size (# bytes)
            memcpy(&column_start[5], col.name, strlen(col.name));  //  Name 

            vector_set_size(column_bytes, length + start_size);
        }

        htops((uint16_t)vector_size(column_bytes) - HEADER_LENGTH, &column_bytes[1]);
        send_buffer(session->socket, column_bytes, vector_size(column_bytes));
        vector_free(column_bytes);
        column_bytes = NULL;
    }
}

static uint8_t * new_rowset_buffer()
{
    vector_type(uint8_t) rowset_bytes = NULL;

    vector_push(rowset_bytes, (uint8_t)NQP_ROWSET);
    vector_push(rowset_bytes, 0x00);
    vector_push(rowset_bytes, 0x00);
    vector_grow(rowset_bytes, MAX_MESSAGE_SIZE);

    return rowset_bytes;
}

static vector_type(uint8_t) say_rowset(struct session *session, vector_type(uint8_t) rowset_bytes)
{
    htops((uint16_t)vector_size(rowset_bytes) - HEADER_LENGTH, &rowset_bytes[1]);
    send_buffer(session->socket, rowset_bytes, vector_size(rowset_bytes));

    vector_free(rowset_bytes);
    return new_rowset_buffer();
}

static void handle_query(struct session *session, size_t payload_size)
{
    char *query = malloc(payload_size);
    struct query_results *results = NULL;
    vector_type(uint8_t) rowset_bytes = NULL;

    receive_buffer(session->socket, (uint8_t *)query, payload_size);
    results = submit_query(session->manager->engine, query, payload_size);

    if (results->parsed_sql->error == true)
    {
        say_completed(session, COMPLETED_FAILURE, results->parsed_sql->error_msg);

        goto cleanup;
    }

    while (get_next_set(results))
    {
        rowset_bytes = new_rowset_buffer();

        if (has_rows(results) == true)
        {
            say_columns(session, results);
        }

        while (next_set_record(results))
        {
            if (vector_size(rowset_bytes) + vector_size(results->current_scan) > MAX_MESSAGE_SIZE)
            {
                rowset_bytes = say_rowset(session, rowset_bytes);
            }

            vector_type(uint8_t) bytes = NULL;
            vector_grow(bytes, MAX_MESSAGE_SIZE);
            vector_type(struct scan_field) fields = results->current_scan->scan_fields;
            for (size_t i = 0; i < vector_size(fields); i++)
            {
                struct scan_field field = fields[i];
                switch (field.type)
                {
                    case TYPE_INT:
                        push_uint32(bytes, field.value.number);
                        break;
                    default:
                        break;
                }
            }
            size_t b_size =  vector_size(bytes);
            uint8_t *row_start = &rowset_bytes[vector_size(rowset_bytes)];
            memcpy(row_start, bytes, b_size);
            vector_increase_size(rowset_bytes, vector_size(bytes));
        }

        if (vector_size(rowset_bytes) > 0)
        {
            rowset_bytes = say_rowset(session, rowset_bytes);
        }
    
    }

    say_completed(session, COMPLETED_SUCCESS, results->parsed_sql->error_msg);

cleanup:
    free_results(results);
    free(query);
    vector_free(rowset_bytes);
    fprintf(stdout, "Returning from query handler\n");
    return;
}

static void return_session(struct session *session)
{
    lock(session->manager->lock);

    free(session->query_loop);

    session->manager->next_open--;
    session->manager->open_sessions[session->manager->next_open] = session->id;
    session->query_loop = NULL;
    
    fprintf(stdout, "Session Id %d has been returned to the pool\n", session->manager->open_sessions[session->manager->next_open]);

    unlock(session->manager->lock);
}
