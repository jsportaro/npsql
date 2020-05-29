#include <common.h>
#include <buffers.h>
#include <file.h>
#include <log_file.h>

static void 
append_new_page(struct log_file *log_file);

static uint16_t 
get_last_record_position(uint8_t *page);

static void 
set_last_record_position(uint16_t position, uint8_t *page);

static void 
flush_page(struct log_file *lf);

static void 
move_to_next_page(struct log_file_iterator *iterator);

void 
open_log_file(struct log_file *lf, const char *p)
{
    lf->file = file_open(p);
    off_t size = file_size(lf->file);

	lf->lock = create_mutex();

    if (size == 0)
    {
        lf->current_page_number = 0;
  
        append_new_page(lf);
    }
    else
    {
        lf->current_page_number = (size / PAGE_SIZE) - 1;
        size_t read;
        file_read(
            lf->file, 
            lf->current_page, 
            PAGE_SIZE, 
            size - PAGE_SIZE, 
            &read);

        lf->current_page_position = get_last_record_position(lf->current_page);
    }
}

void 
close_log_file(struct log_file *log_file)
{
    file_close(log_file->file);
}

LSN 
append(struct log_file *lf, void* r, size_t s)
{
    lock(lf->lock);

    if (lf->current_page_position + s > PAGE_SIZE)
    {
        flush_page(lf);

        lf->current_page_number += 1;
        append_new_page(lf);
    }
    
    memcpy(&(lf->current_page[lf->current_page_position]), r, s);
    lf->current_page_position += (uint16_t)s;
    write_uint16(get_last_record_position(lf->current_page), lf->current_page, lf->current_page_position);
    set_last_record_position(lf->current_page_position, lf->current_page);

    lf->current_page_position += sizeof(uint16_t);

	LSN lsn = lf->current_page_number;

	unlock(lf->lock);

    return lsn;
}

void 
flush_log(struct log_file *lf, LSN lsn)
{
    lock(lf->lock);

    if (lsn >= lf->current_page_number)
    {
        flush_page(lf);
    }

    unlock(lf->lock);
}

void 
open_iterator(struct log_file_iterator *lfi, struct log_file *lf)
{
    lock(lf->lock);
    
    memcpy(lfi->current_page, lf->current_page, PAGE_SIZE);
    
    lfi->currect_position = get_last_record_position(lfi->current_page);
    lfi->current_page_number = lf->current_page_number;
    lfi->log_file = lf;

	unlock(lf->lock);
}

bool
has_next(struct log_file_iterator *lfi)
{
    return lfi->currect_position > 0 || lfi->current_page_number > 0;
}

uint16_t 
next(struct log_file_iterator *lfi)
{
    if (lfi->currect_position == 0)
    {
        move_to_next_page(lfi);
    }

    lfi->currect_position = read_uint16(lfi->current_page, lfi->currect_position);

    return lfi->currect_position + sizeof(uint16_t);
}

static void 
set_last_record_position(uint16_t position, uint8_t *page)
{
    write_uint16(position, page, 0);
}

static uint16_t 
get_last_record_position(uint8_t *page)
{
    return read_uint16(page, 0);
}

static void 
append_new_page(struct log_file *lf)
{
    size_t read = 0;

    file_expand(lf->file, PAGE_SIZE + (lf->current_page_number * PAGE_SIZE)) ;
    file_read(lf->file, lf->current_page, PAGE_SIZE, lf->current_page_number * PAGE_SIZE, &read);
    
    set_last_record_position(0, lf->current_page);
    lf->current_page_position = sizeof(uint16_t);
}

static void 
flush_page(struct log_file *lf)
{
    file_write(lf->file, lf->current_page, PAGE_SIZE, lf->current_page_number * PAGE_SIZE, NULL);
}

static void 
move_to_next_page(struct log_file_iterator *lfi)
{
    lfi->current_page_number--;
    file_read(lfi->log_file->file, lfi->current_page, PAGE_SIZE, lfi->current_page_number * PAGE_SIZE, NULL);

    lfi->currect_position = get_last_record_position(lfi->current_page);
}
