#include <networking.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void start_server(uint16_t port)
{
    fprintf(stdout, "Starting server... ");

    int socket_desc; //,client_socket, c;
    struct sockaddr_in server; //, client;

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

    //while ((client_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&)))

    fprintf(stdout, "started.\n");

    return;

error:
    fprintf(stdout, "failed.  ");

    return;
}
