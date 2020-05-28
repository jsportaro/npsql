#ifndef __NP_FILE_H__
#define __NP_FILE_H__

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int file_open(const char *path);
void file_expand(int fd, off_t size);
off_t file_size(int fd);
void file_delete(const char *file_path);
void file_close(int fd);

void file_read(int fd, void *buffer, uint32_t size, uint64_t page_offset, size_t *bytes_read);
void file_write(int fd, void *buffer, uint32_t size, uint64_t page_offset, size_t *bytes_written);

#endif