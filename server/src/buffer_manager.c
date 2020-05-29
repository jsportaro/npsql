#include <buffer_manager.h>
#include <data_file.h>
#include <log_file.h>
#include <threads.h>

static struct buffer* 
find_existing_buffer(struct buffer_manager *buffer_manager, PNUM page_number);

static struct buffer* 
choose_unpinned_buffer(struct buffer_manager *buffer_manager);

static void 
flush_buffer(struct buffer *buffer);

static void 
read_buffer(struct buffer *buffer, PNUM page_number);

void 
buffer_manager_initialize(
    struct buffer_manager *bf, 
    struct data_file *data_file, 
    struct log_file *log_file)
{
    bf->data_file = data_file;
    bf->log_file = log_file;
    bf->available_buffers = BUFFER_POOL_SIZE;
    bf->lock = create_mutex();

    memset(bf->buffer_pool, 0, sizeof(bf->buffer_pool));
    
    for (uint16_t i = 0; i < BUFFER_POOL_SIZE; i++)
    {
        bf->buffers[i].id = i;
        bf->buffers[i].pins = 0;
        bf->buffers[i].page_number = INVALID_PNUM;
        bf->buffers[i].modified_by = INVALID_TSX;
        bf->buffers[i].owner = bf;
        bf->buffers[i].page = bf->buffer_pool + (i * PAGE_SIZE);
    }
}

void 
free_buffer_manager(struct buffer_manager *bf)
{
    destroy_mutex(bf->lock);
}

struct buffer *
pin(struct buffer_manager *bm, PNUM pnum)
{
    lock(bm->lock);

    struct buffer *b = find_existing_buffer(bm, pnum);

    if (b == NULL)
    {
        b = choose_unpinned_buffer(bm);

        if (b == NULL)
        {
            return NULL;
        }

        flush_buffer(b);
        read_buffer(b, pnum);
    }

    if (b->pins == 0)
    {
        bm->available_buffers--;
    }

    b->pins++;

    unlock(bm->lock);

    return b;
}

struct buffer *
pin_new(struct buffer_manager *bm)
{
    lock(bm->lock);

    PNUM page_number = 0;
    struct buffer *buffer = choose_unpinned_buffer(bm);

    if (buffer == NULL)
    {
        return NULL;
    }

    flush_buffer(buffer);
    append_page(bm->data_file, &page_number, buffer->page);

    buffer->page_number = page_number;
    buffer->pins = 1;
    bm->available_buffers--;

    unlock(bm->lock);

    return buffer;
}

void 
unpin(struct buffer_manager *bm, struct buffer *b)
{
    lock(bm->lock);

    b->pins--;

    if (b->pins == 0)
    {
        bm->available_buffers++;
    }

    unlock(bm->lock);
}

void 
flush_buffers(struct buffer_manager *bm, TSX_NUM tsx)
{
    lock(bm->lock);

    for (int i = 0; i < BUFFER_POOL_SIZE; i++)
    {
        if (bm->buffers[i].modified_by == tsx)
        {
            flush_buffer(&bm->buffers[i]);
        }
    }

    unlock(bm->lock);
}

void 
write_buffer(struct buffer *buffer, void *update, uint16_t offset, size_t length, TSX_NUM tsx, LSN lsn)
{
    buffer->modified_by = tsx;

    if (buffer->lsn != INVALID_LSN)
    {
        buffer->lsn = lsn;
    }

    memcpy(buffer->page + offset, update, length);
}


static struct buffer* 
choose_unpinned_buffer(struct buffer_manager *bm)
{
    for (int i = 0; i < BUFFER_POOL_SIZE; i++)
    {
        if (bm->buffers[i].pins == 0)
        {
            return &bm->buffers[i];
        }
    }

    return NULL;
}

static struct buffer* 
find_existing_buffer(struct buffer_manager *bm, PNUM pnum)
{
    for (int i = 0; i < BUFFER_POOL_SIZE; i++)
    {
        if (bm->buffers[i].page_number == pnum)
        {
            return &bm->buffers[i];
        }
    }

    return NULL;
}

static void 
flush_buffer(struct buffer *buffer)
{
    if (buffer->modified_by != INVALID_TSX)
    {
        flush_log(
            buffer->owner->log_file,
            buffer->lsn);
        write_page(
            buffer->owner->data_file, 
            buffer->page_number, 
            buffer->page);
    }

    buffer->modified_by = INVALID_TSX;
}

static void 
read_buffer(struct buffer *b, PNUM pnum)
{
    read_page(
            b->owner->data_file, 
            pnum, 
            b->page);
    
    b->pins = 0;
}
