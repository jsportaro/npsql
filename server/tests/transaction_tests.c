#include <buffers.h>
#include <buffer_manager.h>
#include <data_file.h>
#include <file.h>
#include <lock_table.h>
#include <transaction.h>
#include <threads.h>

#include <assert.h>
#include <stdio.h>

void 
commit_and_rollback()
{
    char *heather = "Heather Portaro";
    char *rehtaeh = "rehtaeH oratroP";
    const char *data_path = "commit_and_rollback.dat";
    const char *log_path = "commit_and_rollback.log";
    struct transaction_context ctx;
    struct buffer *page = NULL;
    
    file_delete(data_path);
    file_delete(log_path);

    initialize_transaction_context(&ctx, data_path, log_path);
    PNUM page_number = INVALID_PNUM;

    struct transaction *tsx = begin_transaction(&ctx);
    {
        page_number = transactional_append(tsx);
        transactional_pin(tsx, page_number);
        transactional_update(tsx, page_number, heather, 0, strlen(heather));
        transactional_unpin(tsx, page_number);

        commit(tsx);
    }
    
    tsx = begin_transaction(&ctx);
    {
        transactional_pin(tsx, page_number);
        transactional_update(tsx, page_number, rehtaeh, 0, strlen(rehtaeh));
        transactional_unpin(tsx, page_number);

        rollback(tsx);
    }

    page = pin(&ctx.bmgr, page_number);
    assert(strncmp((char *)page->page, heather, strlen(heather)) == 0);
    unpin(&ctx.bmgr, page);

    tsx = begin_transaction(&ctx);
    {
        transactional_pin(tsx, page_number);
        transactional_update(tsx, page_number, rehtaeh, 0, strlen(rehtaeh));
        transactional_unpin(tsx, page_number);

        commit(tsx);
    }

    page = pin(&ctx.bmgr, page_number);
    assert(strncmp((char *)page->page, rehtaeh, strlen(rehtaeh)) == 0);
    unpin(&ctx.bmgr, page);
}

void
multiple_readers()
{
    char *heather = "Heather Portaro";
    char *rehtaeh = "rehtaeH oratroP";
    const char *data_path = "multiple_readers.dat";
    const char *log_path = "multiple_readers.log";
    struct transaction_context ctx;
    struct buffer *page = NULL;

    char buff1[15];
    char buff2[15];
    char buff3[15];

    file_delete(data_path);
    file_delete(log_path);

    initialize_transaction_context(&ctx, data_path, log_path);
    PNUM page_number = INVALID_PNUM;

    struct transaction *tsx = begin_transaction(&ctx);
    {
        page_number = transactional_append(tsx);
        transactional_pin(tsx, page_number);
        transactional_update(tsx, page_number, heather, 0, strlen(heather));
        transactional_unpin(tsx, page_number);

        commit(tsx);
    }

    struct transaction *tsx1 = begin_transaction(&ctx);
    struct transaction *tsx2 = begin_transaction(&ctx);
    struct transaction *tsx3 = begin_transaction(&ctx);

    transactional_pin(tsx1, page_number);
    transactional_pin(tsx2, page_number);
    transactional_pin(tsx3, page_number);

    transactional_read(tsx1, page_number, buff1, 0, 15);
    transactional_read(tsx2, page_number, buff2, 0, 15);
    transactional_read(tsx3, page_number, buff3, 0, 15);

    commit(tsx1);
    commit(tsx2);
    commit(tsx3);
}

struct event *hold_xlock;
struct event *got_slock;

void * 
write_blocking_thread(void* args)
{
    char *heather = "Heather Portaro";
    struct transaction_context *ctx = (struct transaction_context*)args;

    struct transaction* tsx = begin_transaction(ctx);
    {
        transactional_pin(tsx, 0);
        transactional_update(tsx, 0, heather, 0, strlen(heather));
        transactional_unpin(tsx, 0);

        event_wait(hold_xlock);

        commit(tsx);

    }
    return NULL;
}

void * 
read_blocked_thread(void* args)
{
    struct transaction_context *ctx = (struct transaction_context*)args;
    char b[15];
    struct transaction* tsx = begin_transaction(ctx);
    {
        transactional_pin(tsx, 0);
        transactional_read(tsx, 0, b, 0, 15);
        transactional_unpin(tsx, 0);

        event_set(got_slock);

        commit(tsx);
    }
    return NULL;
}

void 
writers_should_block_readers()
{
    //  This test is a little brittle
    //  Should spin till I *know* write thread is in flight
    //  Using sleep now because I'm lazy and I just
    //  want to see it work.
    hold_xlock = create_event();
    got_slock = create_event();
    const char* data_path = "writers_should_block_readers.dat";
    const char* log_path = "writers_should_block_readers.log";
    struct transaction_context ctx;
    struct buffer* page = NULL;

    file_delete(data_path);
    file_delete(log_path);

    initialize_transaction_context(&ctx, data_path, log_path);

    struct buffer *b = pin_new(&ctx.bmgr);

    unpin(&ctx.bmgr, b);

    gpsql_thread write_thread;
    gpsql_thread read_thread;

    write_thread = create_thread(&write_blocking_thread, &ctx);
    read_thread = create_thread(&read_blocked_thread, &ctx);

    event_set(hold_xlock);
    event_set(got_slock);

    join_thread(write_thread, 1000);
    release_thread(write_thread);

    join_thread(read_thread, 1000);
    release_thread(read_thread);

    destroy_event(hold_xlock);
    destroy_event(got_slock);
}

struct event *opened_slock;
struct event *release_slock;
struct event *getting_xlock;
struct event *got_xlock;

void * 
write_blocked_thread(void *args)
{
    char *heather = "Heather Portaro";
    struct transaction_context *ctx = (struct transaction_context*)args;

    struct transaction* tsx = begin_transaction(ctx);
    {
        transactional_pin(tsx, 0);

        event_set(getting_xlock);  //Not perfect
        transactional_update(tsx, 0, heather, 0, strlen(heather));
        event_set(got_xlock);

        transactional_unpin(tsx, 0);
        commit(tsx);

    }
    return NULL;
}

void *
read_blocking_thread(void *args)
{
    struct transaction_context *ctx = (struct transaction_context*)args;
    char b[15];
    struct transaction* tsx = begin_transaction(ctx);
    {
        transactional_pin(tsx, 0);
        transactional_read(tsx, 0, b, 0, 15);

        event_set(opened_slock);
        event_wait(release_slock);

        transactional_unpin(tsx, 0);

        commit(tsx);
    }
    return NULL;
}

void
readers_should_block_writer()
{
    opened_slock = create_event();
    release_slock = create_event();
    getting_xlock = create_event();
    got_xlock = create_event();

    const char* data_path = "readers_should_block_writer.dat";
    const char* log_path = "readers_should_block_writer.log";
    struct transaction_context ctx;
    struct buffer* page = NULL;

    file_delete(data_path);
    file_delete(log_path);

    initialize_transaction_context(&ctx, data_path, log_path);

    //  Prime the data file with just one page
    struct buffer *b = pin_new(&ctx.bmgr);
    unpin(&ctx.bmgr, b);

    gpsql_thread read_thread;
    read_thread = create_thread(&read_blocking_thread, &ctx);

    event_wait(opened_slock);

    gpsql_thread write_thread;
    write_thread = create_thread(&write_blocked_thread, &ctx);

    event_wait(getting_xlock);
    event_set(release_slock);
    event_wait(got_xlock);

    join_thread(read_thread, 1000);
    release_thread(read_thread);

    join_thread(write_thread, 1000);
    release_thread(write_thread);

    destroy_event(opened_slock);
    destroy_event(release_slock);
    destroy_event(getting_xlock);
    destroy_event(got_xlock);
}

void *
transaction_thread(void *args)
{
    char *heather = "Heather Portaro";
    
    struct transaction_context *ctx = (struct transaction_context *)args;

    struct transaction *tsx = begin_transaction(ctx);
    {
        PNUM page_number = transactional_append(tsx);
        transactional_pin(tsx, page_number);
        transactional_update(tsx, page_number, heather, 0, strlen(heather));
        transactional_unpin(tsx, page_number);

        commit(tsx);
    }
    return NULL;
}

void 
hit_it_hard()
{
    const char *data_path = "hit_it_hard.dat";
    const char *log_path = "hit_it_hard.log";
    struct transaction_context ctx;
    struct buffer *page = NULL;
    
    file_delete(data_path);
    file_delete(log_path);

    initialize_transaction_context(&ctx, data_path, log_path);

    gpsql_thread threads[50];

    printf("Create threads\n");

    for (int i = 0; i < 50; i++)
    {
        threads[i] = create_thread(&transaction_thread, &ctx);
    }
    
    printf("Clean up!\n");

    for (int i = 0; i < 50; i++)
    {
        join_thread(threads[i], 1000);
        release_thread(threads[i]);
    }

    printf("Stress test!\n");
}

int main()
{
    commit_and_rollback();
    multiple_readers();
    hit_it_hard();
	writers_should_block_readers();
    readers_should_block_writer();

    return EXIT_SUCCESS;
}