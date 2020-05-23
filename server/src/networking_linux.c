#include <networking.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

struct open_connection
{
    int socked_fd;
};

void server_start(uint16_t port, struct session_manager *session_manager)
{
    fprintf(stdout, "Starting server... ");

    int socket_desc;
    struct sockaddr_in server;

    int s;
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

    while ((s = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&sizeof_sockaddr_in)))
    {
        handle_connection(session_manager, (SOCKET)s);
    }

    return;

error:
    fprintf(stdout, "failed.  ");

    return;
}

void close_connection(SOCKET socket)
{
    close(socket);
}

void send_buffer(SOCKET socket, uint8_t *buffer, size_t size)
{
    write(socket, buffer, size);
}

void receive_buffer(SOCKET socket, uint8_t *buffer, size_t size)
{
    recv(socket, buffer, size, 0);
}
