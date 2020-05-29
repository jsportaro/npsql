#ifndef __LOG_FILE_H__
#define __LOG_FILE_H__

#include <storage.h>
#include <threads.h>

#include <stdbool.h>
#include <stdint.h>

struct log_file
{
    int file;
    uint64_t current_page_number;
    uint16_t current_page_position;

    uint8_t current_page[PAGE_SIZE];

	pthread_mutex_t *lock;
};

struct log_file_iterator
{
    struct log_file *log_file;
    uint8_t current_page[PAGE_SIZE];
    uint16_t currect_position;
    uint64_t current_page_number;
};

void open_log_file(struct log_file *log_file, const char *path);
void close_log_file(struct log_file *log_file);

LSN append(struct log_file *log_file, void* record, size_t size);
void flush_log(struct log_file *log_file, LSN lsn);
void open_iterator(struct log_file_iterator *log_file_iterator, struct log_file *log_file);
bool has_next(struct log_file_iterator *iterator);
uint16_t next(struct log_file_iterator *iterator);

#endif
