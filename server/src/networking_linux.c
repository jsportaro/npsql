#include <networking.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void server_start(uint16_t port, struct session_manager *session_manager)
{
    fprintf(stdout, "Starting server... ");

    int socket_desc;
    struct sockaddr_in server;

    int client_socket;
    struct sockaddr_in client;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc == -1)
    {
        fprintf(stderr, "Could not create socket\n");

        goto error;
    }

    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        fprintf(stderr, "Could not bind\n");

        goto error;
    }

    listen(socket_desc, 10);

    int sizeof_sockaddr_in = sizeof(struct sockaddr_in);

    fprintf(stdout, "started.\n");

    while ((client_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&sizeof_sockaddr_in)))
    {
        session_manager_get_free(session_manager, (void*) &client_socket);
    }

    return;

error:
    fprintf(stdout, "failed.  ");

    return;
}

void send_buffer(void *network_handle, char *buffer, size_t size)
{
    int client_socket = *(int *)network_handle;

    write(client_socket, buffer, size);
}

void receive_buffer(void *network_handle, char *buffer, size_t size)
{
    UNUSED(network_handle);
    UNUSED(buffer);
    UNUSED(size);
}
