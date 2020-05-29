#ifndef __LOG_FILE_H__
#define __LOG_FILE_H__

#include <storage.h>
#include <threads.h>

struct log_file
{
    int file;
    uint64_t current_page_number;
    uint16_t current_page_position;

    char current_page[PAGE_SIZE];

	pthread_mutex_t *lock;
};

struct log_file_iterator
{
    struct log_file *log_file;
    char current_page[PAGE_SIZE];
    uint16_t currect_position;
    uint64_t current_page_number;
};

void open_log_file(struct log_file *log_file, const char *path);
void close_log_file(struct log_file *log_file);


#endif
