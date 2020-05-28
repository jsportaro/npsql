#include <file.h>

#include <assert.h>

int 
file_open(const char *path)
{
    int fd = open(path, O_RDWR | O_CREAT | O_SYNC, 0666);	
    assert(fd != -1);

    return fd;
}

void 
file_expand(int fd, off_t size)
{
    int r = ftruncate(fd, size);
    assert(r == 0);
}

off_t 
file_size(int fd)
{
    off_t s = lseek(fd, 0, SEEK_END);
    assert(s != -1);

    return s;
}


void 
file_delete(const char *file_path)
{
    int r = remove(file_path);
    assert(r != -1);
}

void 
file_close(int fd)
{
    int r = close(fd);
    assert(r != -1);
}

void 
file_read(int fd, void *buffer, uint32_t size, uint64_t page_offset, size_t *bytes_read)
{
    off_t s = lseek(fd, page_offset, SEEK_SET);
    assert(s != -1);

    *bytes_read = read(fd, buffer, size);
    assert(*bytes_read == size);
}

void 
file_write(int fd, void *buffer, uint32_t size, uint64_t page_offset, size_t *bytes_written)
{
    off_t s = lseek(fd, page_offset, SEEK_SET);
    assert(s != -1);

    *bytes_written = write(fd, buffer, size);
    assert(*bytes_written == size);
}
