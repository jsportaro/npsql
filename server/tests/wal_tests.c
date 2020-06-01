#include <assert.h>
#include <buffers.h>
#include <buffer_manager.h>
#include <data_file.h>
#include <file.h>
#include <log_file.h>
#include <wal.h>

void 
should_write_start_and_commit()
{
    const char *data_path = "should_write_start_and_commit.dat";
    const char *log_path = "should_write_start_and_commit.log";
    struct log_file log;
    struct data_file data;
    struct buffer_manager bmgr;
    struct log_file_iterator iterator;
    char *page = calloc(PAGE_SIZE, sizeof(char));
    
    file_delete(data_path);
    file_delete(log_path);

    open_data_file(&data, data_path);
    open_log_file(&log, log_path);
    buffer_manager_initialize(&bmgr, &data, &log);

    page[10] = 'h';
    page[11] = 'e';
    page[12] = 'l';
    page[13] = 'l';
    page[14] = 'o';
    page[0] = 'h';
    page[1] = 'e';
    page[2] = 'a';
    page[3] = 't';
    page[4] = 'h';
    page[5] = 'e';
    page[6] = 'r';


    struct wal_writer *wal = new_wal(1, &log, &bmgr);
    {
        wal_write(wal, 12, 10, &(page[10]), 5);
        wal_write(wal, 12, 0, &(page[0]), 7);
        commit_wal(wal);
    }

    open_iterator(&iterator, wal->log);

    int records = 0;
    bool found_commit = false;
    bool found_start = false;

    while (has_next(&iterator))
    {
        uint16_t next_record = next(&iterator);

        enum log_rec_type type = (enum log_rec_type)iterator.current_page[next_record];
        TSXID tsx_num = read_uint32(iterator.current_page, next_record + 1);

        switch (type)
        {
            case LOG_UPDATE:
                printf("<UPDATE   TSX = %u>\n", tsx_num);
                break;
            case LOG_COMMIT:
                found_commit = true;
                printf("<COMMIT   TSX = %u>\n", tsx_num);
                break;
            case LOG_START:
                found_start = true;
                printf("<START    TSX = %u>\n", tsx_num);
                break;
            case LOG_ROLLBACK:
                printf("<ROLLBACK TSX = %u>\n", tsx_num);
                break;
        }

        records++;
    }

    assert(found_commit);
    assert(found_start);
}

void 
should_rollback()
{
    const char *data_path = "should_rollback.dat";
    const char *log_path = "should_rollback.log";
    struct log_file log;
    struct data_file data;
    struct buffer_manager bmgr;

    file_delete(data_path);
    file_delete(log_path);

    open_data_file(&data, data_path);
    open_log_file(&log, log_path);
    buffer_manager_initialize(&bmgr, &data, &log);

    //  Simulate first transaction
    char first[] =  { 'h', 'e', 'l', 'l', 'o'};
    char second[] = { 'p', 'u', 'p', 'p', 'y'};
    
    struct buffer *first_buffer = pin_new(&bmgr);
    write_buffer(first_buffer, first, 0, sizeof(first), 1, 1);

    struct buffer *second_buffer = pin_new(&bmgr);
    write_buffer(second_buffer, second, 0, sizeof(second), 1, 1);
    int cmp = -1;

    flush_buffers(&bmgr, 1);

    struct wal_writer *wal = new_wal(1, &log, &bmgr);
    {
        wal_write(wal, first_buffer->page_number, 0, first, sizeof(second));
        write_buffer(first_buffer, second, 0, sizeof(second), wal->tsx_id, 1);
        cmp = strncmp((char *)first_buffer->page, second, sizeof(second));

        wal_write(wal, second_buffer->page_number, 0, second, sizeof(first));
        write_buffer(second_buffer, first, 0, sizeof(first), wal->tsx_id, 1);
        cmp = strncmp((char *)second_buffer->page, first, sizeof(first));

        rollback_wal(wal);
    }
   
    cmp = strncmp((char *)first_buffer->page, first, 5);
    cmp = strncmp((char *)second_buffer->page, second, 5);
}

int main(void)
{
    should_write_start_and_commit();
    should_rollback();
}