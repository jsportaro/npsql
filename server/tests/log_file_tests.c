#include <buffers.h>
#include <file.h>
#include <log_file.h>

struct example_log_file
{
    uint32_t number;
    uint16_t size;
    char* string;
};

void 
new_log_should_write_records()
{
    struct log_file log_file;
    const char *path = "should_write_records.log";
    LSN lsn = 0;
    file_delete(path);

    int a[] = { 0x80000008, 0x80000008 };

    open_log_file(&log_file, path);
    lsn = append(&log_file, &a, sizeof(a));
    lsn = append(&log_file, &a, sizeof(a));
    flush_log(&log_file, lsn);
    close_log_file(&log_file);

    assert(log_file.current_page_position == 22);
}

void 
existing_log_should_write_records()
{
    struct log_file log_file, existing_log;
    const char *path = "existing_log_should_write_records.log";
    LSN lsn = 0;
    file_delete(path);

    int a[] = { 0x80000008, 0x80000008 };

    open_log_file(&log_file, path);
    lsn = append(&log_file, &a, sizeof(a));
    lsn = append(&log_file, &a, sizeof(a));
    flush_log(&log_file, lsn);
    close_log_file(&log_file);

    open_log_file(&existing_log, path);
    lsn = append(&existing_log, &a, sizeof(a));
    flush_log(&existing_log, lsn);
    close_log_file(&existing_log);

    assert(existing_log.current_page_position == 30);
}

void
should_append_new_page_when_current_full()
{
    struct log_file log_file;
    const char *path = "should_append_new_page_when_current_full.log";
    LSN lsn = 0;
    file_delete(path);

    char data = 0xFF;

    open_log_file(&log_file, path);

    //  With the back tracking offsets, each log entry
    //  should take 3 bytes.  Hence, the magic three
    //  below.  Just want to write one more page so the magic
    //  10 should get us to the second page (page number 1)
    for (int i = 0; i < (PAGE_SIZE / 3) + 10 ; i++)
    {
        if (i % PAGE_SIZE == 0)
        {
            printf("New page time\n");
        }
        lsn = append(&log_file, &data, sizeof(data));
    }


    assert(log_file.current_page_number == 1);

    flush_log(&log_file, lsn);
    close_log_file(&log_file);
}

void
check_for_last_offsets()
{
    struct log_file log_file;
    const char *path = "check_for_last_offsets.log";
    LSN lsn = 0;
    file_delete(path);

    char *first = "ab";
    char *second= "cd";
    char *third = "ef";

    open_log_file(&log_file, path);
    lsn = append(&log_file, first, strlen(first));
    lsn = append(&log_file, second, strlen(second));
    lsn = append(&log_file, third, strlen(third));
    flush_log(&log_file, lsn);
    close_log_file(&log_file);
    
    assert(log_file.current_page_position == 14);
}

void
should_iterate_through_log()
{
    struct log_file log_file;
    struct log_file_iterator iterator;
    
    const char *path = "should_iterate_through_log.log";
    LSN lsn = 0;
    file_delete(path);

    char first[] = { 'a', 'b' };
    char second[] = { 'c', 'd' };
    char third[] = { 'e', 'f' };

    int expect_records_at[] = { 10, 6, 2 };

    open_log_file(&log_file, path);
    lsn = append(&log_file, first, sizeof(first));
    lsn = append(&log_file, second, sizeof(second));
    lsn = append(&log_file, third, sizeof(third));
    flush_log(&log_file, lsn);

    open_iterator(&iterator, &log_file);

    int   i = 0;

    while (has_next(&iterator))
    {
        uint16_t next_record = next(&iterator);

        printf("next record is at %d\n", next_record);
        assert(next_record == expect_records_at[i]);

        i++;
    }

    close_log_file(&log_file);

}

void
should_iterate_through_page_spanning_log()
{
    struct log_file log_file;
    struct log_file_iterator iterator;
    
    const char *path = "should_iterate_through_page_spanning_log.log";
    LSN lsn = 0;
    file_delete(path);

    char *string = "Testing page spanning log files.";
    size_t length = strlen(string);

    open_log_file(&log_file, path);

    int record_count = (PAGE_SIZE / (int)length) + 3;

    for (int j = 0; j < record_count; j++)
    {
        lsn = append(&log_file, string, length);
    }

    flush_log(&log_file, lsn);

    open_iterator(&iterator, &log_file);

    int i = 0;

    while (has_next(&iterator))
    {
        uint16_t next_record = next(&iterator);

        i++;
    }

    assert(record_count == i);

    close_log_file(&log_file);
}

int main(void)
{
    new_log_should_write_records();
    existing_log_should_write_records();
    should_append_new_page_when_current_full();
    check_for_last_offsets();
    should_iterate_through_log();
    should_iterate_through_page_spanning_log();
}
