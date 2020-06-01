#ifndef __BUFFER_MANAGER_H__
#define __BUFFER_MANAGER_H__

#include <defaults.h>
#include <storage.h>
#include <threads.h>

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BUFFER_POOL_SIZE 100

struct buffer_manager;

struct buffer
{
    uint16_t id;
    uint16_t pins;
    PNUM     page_number;
    TSXID    modified_by;
    LSN      lsn;
    uint8_t  *page;

    struct buffer_manager *owner;
};

struct buffer_manager
{
    struct data_file *data_file;
    struct log_file *log_file;
    uint16_t available_buffers;

    struct buffer buffers[BUFFER_POOL_SIZE];
    uint8_t buffer_pool[BUFFER_POOL_SIZE * PAGE_SIZE];

    pthread_mutex_t *lock;
};

void buffer_manager_initialize(
    struct buffer_manager *buffer_manager, 
    struct data_file *data_file, 
    struct log_file *log_file);
void free_buffer_manager(struct buffer_manager *buffer_manager);

struct buffer *pin(struct buffer_manager *buffer_manager, PNUM page_number);
struct buffer *pin_new(struct buffer_manager *buffer_manager);
void unpin(struct buffer_manager *buffer_manager, struct buffer *buffer);
void flush_buffers(struct buffer_manager *buffer_manager, TSXID tsx);
void write_buffer(struct buffer *buffer, void *update, uint16_t offset, size_t length, TSXID tsx, LSN lsn);

#endif
