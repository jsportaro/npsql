#ifndef __DATA_FILE_H__
#define __DATA_FILE_H__

#include <storage.h>
#include <threads.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define INITIAL_PAGE_COUNT 100
#define INITIAL_DATA_FILE_SIZE (PAGE_SIZE * INITIAL_PAGE_COUNT)

typedef uint64_t block_id_t;

struct data_file
{
    int file;
    off_t size;
    uint64_t page_count;

    pthread_mutex_t *lock;
};

void data_file_open(struct data_file *data_file, const char *path);
void data_file_close(struct data_file *data_file);
uint64_t page_count(struct data_file *data_file);
void append_page(struct data_file *data_file, PNUM *page_number, uint8_t *page);
void write_page(struct data_file *data_file, PNUM page_number, uint8_t *page);
void read_page(struct data_file *data_file,  PNUM page_number, uint8_t *page);

#endif
