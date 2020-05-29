#include <common.h>
#include <file.h>
#include <log_file.h>

static void 
append_new_page(struct log_file *log_file);

static uint16_t 
get_last_record_position(char *page);

void open_log_file(struct log_file *log_file, const char *path)
{
    log_file->file = file_open(path);
    off_t size = file_size(log_file->file);

	log_file->lock = create_mutex();

    if (size == 0)
    {
        log_file->current_page_number = 0;
  
        append_new_page(log_file);
    }
    else
    {
        log_file->current_page_number = (size / PAGE_SIZE) - 1;
        size_t read;
        file_read(
            log_file->file, 
            log_file->current_page, 
            PAGE_SIZE, 
            size - PAGE_SIZE, 
            &read);

        log_file->current_page_position = get_last_record_position(log_file->current_page);
    }
}

void close_log_file(struct log_file *log_file)
{
    file_close(log_file->file);
}


static void 
append_new_page(struct log_file *log_file)
{
    UNUSED(log_file);
    // uint32_t read = 0;

    // int r = ftruncate(log_file->file, (off_t)size);
    // assert(r == 0);

    // file_expand(log_file->file, PAGE_SIZE);
    // file_read(log_file->file, log_file->current_page, PAGE_SIZE, log_file->current_page_number * PAGE_SIZE, &read);
    
    // set_last_record_position(0, log_file->current_page);
    // log_file->current_page_position = sizeof(uint16_t);
}
static uint16_t get_last_record_position(char *page)
{
    UNUSED(page);
    return 0;
    //return read_uint16(page, 0);
}
