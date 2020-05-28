#include <buffer_manager.h>
#include <threads.h>

// static struct buffer* 
// find_existing_buffer(struct buffer_manager *bf, PNUM pn);

// static struct buffer* 
// choose_unpinned_buffer(struct buffer_manager *bf);

void 
buffer_manager_initialize(
    struct buffer_manager *bf)
{
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

// struct buffer *
// pin(struct buffer_manager *bf, PNUM pn)
// {
//     lock(bf->lock);

//     struct buffer *b = find_existing_buffer(bf, pn);

//     if (b == NULL)
//     {
//         b = choose_unpinned_buffer(bf);

//         if (b == NULL)
//         {
//             return NULL;
//         }

//         flush_buffer(b);
//         read_buffer(b, pn);

//     }

//     if (b->pins == 0)
//     {
//         bf->available_buffers--;
//     }

//     b->pins++;

//     unlock(bf->lock);

//     return b;
// }



// static struct buffer* 
// choose_unpinned_buffer(struct buffer_manager *bf)
// {
//     for (int i = 0; i < BUFFER_POOL_SIZE; i++)
//     {
//         if (bf->buffers[i].pins == 0)
//         {
//             return &bf->buffers[i];
//         }
//     }

//     return NULL;
// }

// static struct buffer* 
// find_existing_buffer(struct buffer_manager *bf, PNUM pn)
// {
//     for (int i = 0; i < BUFFER_POOL_SIZE; i++)
//     {
//         if (bf->buffers[i].page_number == pn)
//         {
//             return &bf->buffers[i];
//         }
//     }

//     return NULL;
// }

// static void flush_buffer(struct buffer *b)
// {
//     if (b->modified_by != INVALID_TSX)
//     {
// 		flush_log(
// 			b->owner->log_file,
// 			buffer->lsn);
//         write_page(
//             b->owner->data_file, 
//             b->page_number, 
//             b->page);
//     }

//     b->modified_by = INVALID_TSX;
// }
