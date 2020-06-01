#ifndef __WAL_H__
#define __WAL_H__

#include <storage.h>
#include <transaction.h>

enum log_rec_type
{
    LOG_START = 1,
    LOG_COMMIT = 2,
    LOG_UPDATE = 3,
    LOG_ROLLBACK = 4
};

struct wal_writer
{
    TSXID tsx_id;
    struct log_file *log;
    struct buffer_manager *bmgr;
};

struct wal_writer *new_wal(TSXID tsx_id, struct log_file *log_file, struct buffer_manager *buffer_manager);

void commit_wal(struct wal_writer *wal);
void rollback_wal(struct wal_writer *wal);
LSN wal_write(struct wal_writer *wal, PNUM pnum, uint16_t offset, void* old, size_t length);

#endif
