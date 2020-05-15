#ifndef __NETWORKING_H__
#define __NETWORKING_H__

#include <common.h>
#include <nqp.h>
#include <npsql.h>
#include <threads.h>

#include <stdint.h>
#include <stdio.h>

void server_start(uint16_t port, struct session_manager *session_manager);
void close_connection(SOCKET socket);

void send_buffer(SOCKET socket, uint8_t *buffer, size_t size);
void receive_buffer(SOCKET socket, uint8_t *buffer, size_t size);

#endif
