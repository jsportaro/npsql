#include <networking.h>

char welcome[5];
char sorry[3];

// static void intToNetwork(const int32_t value, char *buffer)
// {
//     const int i = 1;
//     const char *c = (char *)&value;

//     if (( (*(char*)&i) == 1 )) {
//         buffer[0] = c[0];
//         buffer[1] = c[1];
//         buffer[2] = c[2];
//         buffer[3] = c[3];
//     } else {
//         buffer[0] = c[3];
//         buffer[1] = c[2];
//         buffer[2] = c[1];
//         buffer[3] = c[0];
//     }
// }

static void shortToNetwork(const int16_t value, char *buffer)
{
    const int i = 1;
    const char *c = (char *)&value;

    if (( (*(char*)&i) == 1 )) {
        buffer[0] = c[0];
        buffer[1] = c[1];
    } else {
        buffer[0] = c[3];
        buffer[1] = c[2];
    }
}

void session_manager_init(struct session_manager *session_manager)
{
    welcome[0] = (char)NQP_WELCOME;
    shortToNetwork(sizeof(welcome), &welcome[1]);
    shortToNetwork(MAX_MESSAGE_SIZE, &welcome[3]);

    sorry[0] = (char)NQP_SORRY;
    shortToNetwork(0, &sorry[1]);


    for (int i = 0; i < MAX_SESSION_COUNT; i++)
    {
        session_manager->open_sessions[i] = i;
        session_manager->sessions[i].id = i;
    }

    session_manager->next_open = 0;
    session_manager->lock = create_mutex();
}

void session_manager_get_free(struct session_manager *session_manager, void *network_handle)
{
    lock(session_manager->lock);

    if (session_manager->next_open == MAX_SESSION_COUNT)
    {
        fprintf(stdout, "Saying sorry\n");
        send_buffer(network_handle, sorry, sizeof(sorry));

        goto clean_up;
    }

    const int free_index = session_manager->open_sessions[session_manager->next_open];

    session_manager->sessions[free_index].network_handle = network_handle;
    session_manager->next_open++;

    fprintf(stdout, "Saying welcome\n");
    send_buffer(network_handle, welcome, sizeof(welcome));

clean_up:
    unlock(session_manager->lock);

    return;
}

void session_start(struct session *session)
{
    UNUSED(session);
}