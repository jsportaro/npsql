#include <common.h>
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

    UNUSED(r);
}

off_t 
file_size(int fd)
{
    off_t s = lseek(fd, 0, SEEK_END);
    assert(s != -1);
    UNUSED(S);

    return s;
}

void 
file_delete(const char *file_path)
{
    if (access(file_path, F_OK ) != -1)
    {
        int r = remove(file_path);
        assert(r != -1);

        UNUSED(r);
    }
}

void 
file_close(int fd)
{
    int r = close(fd);
    assert(r != -1);

    UNUSED(r);
}

void 
file_read(int fd, void *buffer, uint32_t size, uint64_t page_offset, size_t *bytes_read)
{
    uint32_t br = 0;
    off_t s = lseek(fd, page_offset, SEEK_SET);
    assert(s != -1);

    br = read(fd, buffer, size);
    assert(br == size);
    UNUSED(br);

    if (bytes_read != NULL)
    {
        *bytes_read = size;
    }
}

void 
file_write(int fd, void *buffer, uint32_t size, uint64_t page_offset, size_t *bytes_written)
{
    uint32_t bw = 0;
    off_t s = lseek(fd, page_offset, SEEK_SET);
    assert(s != -1);
    UNUSED(s);

    bw = write(fd, buffer, size);
    assert(bw == size);
    UNUSED(bw);
    
    if (bytes_written != NULL)
    {
        *bytes_written = size;
    }
}
