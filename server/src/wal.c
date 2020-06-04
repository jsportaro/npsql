#include <assert.h>
#include <buffers.h>
#include <buffer_manager.h>
#include <common.h>
#include <log_file.h>
#include <storage.h>
#include <transaction.h>
#include <vector.h>
#include <wal.h>

#define RECORD_HEADER_LENGTH 5

// WAL Record Layout
// | 0  1  2  3  4  5  6  7  8  |  9  10 11 12  |  13 14 15 16 ...
// |            PNUM            |  PAGE OFFSET  |  UPDATE DATA ...
#define WAL_PNUM_POS        0
#define WAL_PAGE_OFFSET_POS 8
#define WAL_LENGTH_POS      10
#define WAL_REC_PAYLOAD_POS(off) ((off) + 12)
#define WAL_UPDATE_REC_LEN(s)   \
    RECORD_HEADER_LENGTH + WAL_PNUM_POS + WAL_PAGE_OFFSET_POS + WAL_LENGTH_POS + (s)

static LSN 
record_commit(struct wal_writer *wal);

static void 
do_rollback(struct wal_writer *wal);

static void 
undo_update(
    struct log_file_iterator *i, 
    uint16_t offset, 
    struct wal_writer *wal);

static LSN 
record_rollback(struct wal_writer *wal);

static void 
record_start(struct wal_writer *wal);

struct wal_writer *
new_wal(TSXID tsx_id, struct log_file *log_file, struct buffer_manager *buffer_manager)
{
    struct wal_writer *wal = malloc(sizeof(struct wal_writer));

    assert(wal != NULL);

    wal->tsx_id = tsx_id;
    wal->log    = log_file;
    wal->bmgr   = buffer_manager;

    record_start(wal);

    return wal;
}

void 
commit_wal(struct wal_writer *wal)
{
    LSN commit_lsn = record_commit(wal);

    flush_log(wal->log, commit_lsn);
}

void 
rollback_wal(struct wal_writer *wal)
{
    do_rollback(wal);
    flush_buffers(wal->bmgr, wal->tsx_id);
    LSN rollback_lsn = record_rollback(wal);
    flush_log(wal->log, rollback_lsn);    
}

LSN 
wal_write(struct wal_writer *wal, PNUM pnum, uint16_t offset, void* old, size_t length)
{
    size_t record_length = WAL_UPDATE_REC_LEN(length);

    vector_type(uint8_t) update_record = NULL;
    vector_grow(update_record, record_length);
    
    LSN commit_lsn = INVALID_LSN;

    vector_push(update_record, LOG_UPDATE);
    push_uint32(update_record, wal->tsx_id);
    push_uint64(update_record, pnum);
    push_uint16(update_record, offset);
    push_uint16(update_record, length);
    push_cpy(update_record, old, length);

    commit_lsn = append(wal->log, update_record, record_length);

    vector_free(update_record);

    return commit_lsn;
}

static LSN 
record_commit(struct wal_writer *wal)
{
    uint8_t commit_rec[RECORD_HEADER_LENGTH] = { 0 };

    commit_rec[0] = LOG_COMMIT;
    write_uint32(commit_rec, 1, wal->tsx_id);

    LSN commit_lsn = append(wal->log, &commit_rec, RECORD_HEADER_LENGTH);

    return commit_lsn;
}

static void undo_update(
    struct log_file_iterator *i, 
    uint16_t offset, 
    struct wal_writer *wal)
{
    uint16_t start = offset + RECORD_HEADER_LENGTH;
    uint8_t  *update_offset = &i->current_page[start];

    uint64_t page_number = read_uint64(update_offset, WAL_PNUM_POS);
    uint16_t page_offset = read_uint16(update_offset, WAL_PAGE_OFFSET_POS);
    uint16_t length      = read_uint16(update_offset, WAL_LENGTH_POS);
    uint8_t  *entry       = calloc(length, sizeof(uint8_t));

    memcpy(entry, WAL_REC_PAYLOAD_POS(update_offset), length);

    struct buffer *buffer = pin(wal->bmgr, page_number);
    write_buffer(buffer, entry, page_offset, length, wal->tsx_id, INVALID_LSN);
    unpin(wal->bmgr, buffer);

	free(entry);
}

static void 
do_rollback(struct wal_writer *wal)
{
    struct log_file_iterator i;
    open_iterator(&i, wal->log);

    while (has_next(&i))
    {
        uint16_t next_record = next(&i);
        enum log_rec_type record_type = (enum log_rec_type)i.current_page[next_record];
        TSXID record_tsx = read_uint32(i.current_page, next_record + 1);

        if (record_tsx == wal->tsx_id)
        {
            switch (record_type)
            {
                case LOG_UPDATE:
                    undo_update(&i, next_record, wal);
                    break;
                case LOG_START:
                    return;
                case LOG_COMMIT:
                    //  This should NEVER happen
                    assert(false);
                default:
                    assert(false);
                    break;
            }
        }
    }
}

static LSN record_rollback(struct wal_writer *wal)
{
    uint8_t rollback_rec[RECORD_HEADER_LENGTH] = { 0 };

    rollback_rec[0] = LOG_ROLLBACK;
    write_uint32(rollback_rec, 1, wal->tsx_id);

    LSN rollback_lsn = append(wal->log, rollback_rec, RECORD_HEADER_LENGTH);

    return rollback_lsn;
}

static void 
record_start(struct wal_writer *wal)
{
    uint8_t start_rec[RECORD_HEADER_LENGTH] = { 0 };

    start_rec[0] = LOG_START;
    write_uint32(start_rec, 1, wal->tsx_id);

    append(wal->log, start_rec, RECORD_HEADER_LENGTH);
}
