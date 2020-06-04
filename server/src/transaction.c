#include <buffer_manager.h>
#include <data_file.h>
#include <log_file.h>
#include <transaction.h>
#include <wal.h>

#include <stdatomic.h>

static void 
unpin_all(struct transaction *tsx);

static void 
release_locks(struct transaction *tsx);

static void 
cleanup(struct transaction *tsx);

static struct 
transaction_buffer* get_buffer(struct transaction *tsx, PNUM pnum);

static struct buffer* 
add_buffer(struct transaction *tsx, struct buffer *buffer, bool pin);

static void 
get_lock(struct transaction *tsx, PNUM page_number, bool is_shared);

static bool 
lock_already_taken(struct transaction *tsx, PNUM page_number);

void 
initialize_transaction_context(
    struct transaction_context *ctx, 
    const char *data_file_path, 
    const char *log_file_path)
{
    open_data_file(&ctx->data, data_file_path);
    open_log_file(&ctx->log, log_file_path);
    buffer_manager_initialize(&ctx->bmgr, &ctx->data, &ctx->log);
    create_lock_table(&ctx->locks);

    ctx->tsxid_lock = create_mutex();
}

void 
free_transaction_context(struct transaction_context *ctx)
{
    destroy_mutex(ctx->tsxid_lock);
    free_lock_table(&ctx->locks);
    free_buffer_manager(&ctx->bmgr);
    close_log_file(&ctx->log);
    close_data_file(&ctx->data);
}

struct transaction*  
begin_transaction(struct transaction_context *context)
{
    struct transaction *tsx = malloc(sizeof(struct transaction));

    assert(tsx != NULL);

    lock(context->tsxid_lock);
    tsx->id = context->next;
    context->next++;
    unlock(context->tsxid_lock);

    tsx->ctx = context;
    tsx->buffers = NULL;
    tsx->locks = NULL;
    tsx->wal = new_wal(tsx->id, &context->log, &context->bmgr);

    return tsx;
}

void 
commit(struct transaction *tsx)
{
    unpin_all(tsx);
    //  This should not be strictly necessary 
    //  As the unpinned pages are removed from the buffer
    //  they should only be flushed after the log records
    //  pertaining to the tsx have been flushed
    flush_buffers(&tsx->ctx->bmgr, tsx->id);
    commit_wal(tsx->wal);
    
    cleanup(tsx);
}

void 
rollback(struct transaction *tsx)
{
    unpin_all(tsx);
    rollback_wal(tsx->wal);

    cleanup(tsx);
}

void 
transactional_pin(struct transaction *tsx, PNUM pnum)
{
    struct transaction_buffer *buffer = get_buffer(tsx, pnum);

    if (buffer != NULL)
    {
        if (!buffer->is_pinned)
        {
            struct buffer *b = pin(&tsx->ctx->bmgr, pnum);

			if (b == NULL)
			{
				return;
			}
        }

        return;
    }

    // Not in buffers list so add and pin
    add_buffer(tsx, pin(&tsx->ctx->bmgr, pnum), true);
}

void 
transactional_unpin(struct transaction *tsx, PNUM pnum)
{
    struct transaction_buffer *buffer = get_buffer(tsx, pnum);

    if (buffer != NULL)
    {
        if (buffer->is_pinned)
        {
            unpin(&tsx->ctx->bmgr, buffer->buffer);
            buffer->is_pinned = false;
        }

        return;
    }
}

void 
transactional_read(
    struct transaction *tsx, 
    PNUM pnum, 
    void* dest, 
    uint16_t offset,
    size_t length)
{
    get_lock(tsx, pnum, true);
    struct transaction_buffer *tbuffer = get_buffer(tsx, pnum);

    memcpy(dest, tbuffer->buffer->page + offset, length);
}

void transactional_update(
    struct transaction *tsx, 
    PNUM pnum, 
    void* src, 
    uint16_t offset, 
    size_t length)
{
    get_lock(tsx, pnum, false);
    struct transaction_buffer *tbuffer = get_buffer(tsx, pnum);

    if (tbuffer == NULL)
    {
        return;
    }
    
    LSN lsn = wal_write(tsx->wal, pnum, offset, tbuffer->buffer->page, length);
    write_buffer(tbuffer->buffer, src, offset, length, tsx->id, lsn);
}

PNUM transactional_append(struct transaction *tsx)
{
    struct buffer *new = pin_new(&tsx->ctx->bmgr);
    add_buffer(tsx, new, true);

    return new->page_number;
}

static void 
unpin_all(struct transaction *tsx)
{
    for (size_t i = 0; i < vector_size(tsx->buffers); i++)
    {
        if (tsx->buffers[i].is_pinned == true)
        {
            unpin(&tsx->ctx->bmgr, tsx->buffers[i].buffer);
            tsx->buffers[i].is_pinned = false;
        }
    }
}

static void 
release_locks(struct transaction *tsx)
{
    for (size_t i = 0; i < vector_size(tsx->locks); i++)
    {
        release(&tsx->ctx->locks, tsx->locks[i]);
    }
}

static void 
cleanup(struct transaction *tsx)
{
    release_locks(tsx);
    vector_free(tsx->buffers);
    vector_free(tsx->locks);
    free(tsx->wal);
    free(tsx);
}

static struct 
transaction_buffer* get_buffer(struct transaction *tsx, PNUM pnum)
{
    for (size_t i = 0; i < vector_size(tsx->buffers); i++)
    {
        if (tsx->buffers[i].buffer->page_number == pnum)
        {
            return &tsx->buffers[i];
        }
    }

    return NULL;
}

static struct buffer* 
add_buffer(struct transaction *tsx, struct buffer *buffer, bool pin)
{
    struct transaction_buffer b;

    b.buffer    = buffer;
    b.is_pinned = pin;
    b.pins      = 1;

    vector_push(tsx->buffers, b);
   
    return buffer;
}


static void 
get_lock(struct transaction *tsx, PNUM pnum, bool is_shared)
{
    if (lock_already_taken(tsx, pnum))
    {
        return;
    }

    if (is_shared == true)
    {
        slock(&tsx->ctx->locks, pnum);
    }
    else
    {
        xlock(&tsx->ctx->locks, pnum);
    }

    vector_push(tsx->locks, pnum);
}

static bool 
lock_already_taken(struct transaction *tsx, PNUM pnum)
{
    for (size_t i = 0; i < vector_size(tsx->locks); i++)
    {
        if (tsx->locks[i] == pnum)
        {
            return true;
        }
    }

    return false;
}
