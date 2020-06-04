#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__

#include <buffer_manager.h>
#include <data_file.h>
#include <log_file.h>
#include <lock_table.h>
#include <storage.h>
#include <threads.h>
#include <vector.h>
#include <wal.h>

#include <stdatomic.h>

struct transaction_context
{
    struct log_file log;
    struct data_file data;
    struct buffer_manager bmgr;
    struct lock_table locks;

    pthread_mutex_t *tsxid_lock;
    TSXID next;
};

struct transaction_buffer
{
    struct buffer *buffer;
    bool is_pinned;
    uint32_t pins;
};

struct transaction
{
    TSXID id;

    struct transaction_context *ctx;
    struct wal_writer *wal;

    vector_type(struct transaction_buffer) buffers;
    vector_type(PNUM) locks;
};

void initialize_transaction_context(
    struct transaction_context *ctx, 
    const char *data_file_path, 
    const char *log_file_path);

void free_transaction_context(struct transaction_context *ctx);

struct transaction * begin_transaction(struct transaction_context *context);
void commit(struct transaction *transaction);
void rollback(struct transaction *transaction);

// Not sure these should be exposed.  Maybe part of r/w functions?
void transactional_pin(struct transaction *transaction, PNUM page_number);
void transactional_unpin(struct transaction *transaction, PNUM page_number);

void transactional_read(
    struct transaction *transaction, 
    PNUM page_number, 
    void* dest, 
    uint16_t offset,
    size_t length);

void transactional_update(
    struct transaction *transaction, 
    PNUM page_number, 
    void* src, 
    uint16_t offset, 
    size_t length);

PNUM transactional_append(struct transaction *transaction);

#endif
