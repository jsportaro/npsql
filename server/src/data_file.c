#include <data_file.h>
#include <file.h>
#include <threads.h>

void 
data_file_open(struct data_file *df, const char *path)
{
    df->file = file_open(path);
    df->size = file_size(df->file);
    df->page_count = df->size / PAGE_SIZE;

    df->lock = create_mutex();
}

void 
data_file_close(struct data_file *df)
{
    file_close(df->file);
}

uint64_t 
page_count(struct data_file *df)
{
    return df->page_count;
}

void 
append_page(struct data_file *df, PNUM *pnum, uint8_t *p)
{
    lock(df->lock);

    *pnum = df->page_count;
    df->page_count++;
    
    write_page(df, *pnum, p);
    unlock(df->lock);
}

void
write_page(struct data_file *data_file, PNUM pnum, uint8_t *page)
{
    uint64_t offset = pnum * PAGE_SIZE;
    
    file_write(data_file->file, page, PAGE_SIZE, offset, NULL);
}

void 
read_page(struct data_file *data_file,  PNUM page_number, uint8_t *page)
{
    uint64_t page_offset = page_number * PAGE_SIZE;
    
    file_read(data_file->file, page, PAGE_SIZE, page_offset, NULL);
}