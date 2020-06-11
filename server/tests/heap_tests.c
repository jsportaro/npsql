#include <assert.h>
#include <buffers.h>
#include <buffer_manager.h>
#include <data_file.h>
#include <data_page.h>
#include <file.h>
#include <lock_table.h>
#include <heap_table.h>
#include <storage.h>
#include <transaction.h>
#include <threads.h>
#include <types.h>
#include <value.h>


void 
single_table_page_write_read()
{
    const char *data_path = "single_table_page_write_read.dat";
    const char *log_path = "single_table_page_write_read.log";
    struct transaction_context ctx = { 0 };
    struct data_page page = { 0 };
    struct data_page existing_page = { 0 };

    file_delete(data_path);
    file_delete(log_path);

    initialize_transaction_context(&ctx, data_path, log_path);
    PNUM page_number = INVALID_PNUM;

    struct table_info people = { 0 };
    init_table_info(&people);

    add_char(&people, "name", 4, 15);
    add_int(&people, "age", 3);
    struct transaction *tsx = NULL;
    tsx = begin_transaction(&ctx);
    {
        page_number = new_data_page(&page, &people, tsx);

        RECORD_HANDLE handle = data_page_insert(&page);
        if (handle != INVALID_RECORD_HANDLE)
        {
            page_set_char(&page, handle, "name", "Heather Portaro");
            page_set_int(&page, handle, "age", 34);
        }

        handle = data_page_insert(&page);
        if (handle != INVALID_RECORD_HANDLE)
        {
            page_set_char(&page, handle, "name", "Heather Portaro");
            page_set_int(&page, handle, "age", 34);
        }

        handle = data_page_insert(&page);
        if (handle != INVALID_RECORD_HANDLE)
        {
            page_set_char(&page, handle, "name", "Joseph Portaro");
            page_set_int(&page, handle, "age", 36);
        }

        commit(tsx);
    }

    tsx = begin_transaction(&ctx);
    {
        struct data_page_iterator iterator;
        open_data_page(page_number, &existing_page, &people, tsx);

        open_page_iterator(&existing_page, &iterator);
        RECORD_HANDLE handle = INVALID_RECORD_HANDLE;
        while((handle = next_page_record(&iterator)) != INVALID_RECORD_HANDLE)
        {
            int age;
            char name[16];

            page_get_int(&existing_page, handle, "age", &age);
            printf("Age: %d\n", age);

            page_get_char(&existing_page, handle, "name", name);
            name[15] = '\0';
            printf("Age : %s\n\n", name);
        }

        commit(tsx);
    }

    free_transaction_context(&ctx);
}

void
complete_table()
{
    const char *data_path = "complete_table.dat";
    const char *log_path = "complete_table.log";
    struct transaction_context ctx = { 0 };
    
    file_delete(data_path);
    file_delete(log_path);

    initialize_transaction_context(&ctx, data_path, log_path);
    PNUM page_number = INVALID_PNUM;

    struct table_info people_info = { 0 };

    init_table_info(&people_info);

    add_char(&people_info, "name", 4, 15);
    add_int(&people_info, "age", 3);

    int written = 0;
    struct transaction *write_tsx = begin_transaction(&ctx);
    {
        struct heap_table people = { 0 };

        create_heap_table(&people, &people_info, write_tsx);

        for (int i = 0; i < (23 * 70); i++) //
        {
            struct record_id rid = heap_insert(&people);

            set_char(&people, rid, "name", "Heather Portaro");
            set_int(&people, rid, "age", i);

            written++;
        }
        
        commit(write_tsx);
    }

    int read = 0;
    struct transaction *read_tsx = begin_transaction(&ctx);
    {
        struct heap_table read_table;
        struct heap_iterator iterator;

        open_heap_table(&read_table, &people_info, read_tsx, 0);
        open_heap_iterator(&read_table, &iterator);

        int age = 0;
        char name[16] = { 0 };

        while (next_record(&iterator))
        {
            struct record_id current_rid = iterator.current_record;

            get_int(&read_table, current_rid, "age", &age);
            //printf("Age: %d\n", age);

            get_char(&read_table, current_rid, "name", name);
            name[15] = '\0';
            //printf("Age : %s\n\n", name);

            read++;
        }

        commit(read_tsx);
    }

    assert(written == read);

    free_transaction_context(&ctx);
}

void
read_table_with_values()
{
    const char *data_path = "complete_table.dat";
    const char *log_path = "complete_table.log";
    struct transaction_context ctx = { 0 };
    
    file_delete(data_path);
    file_delete(log_path);

    initialize_transaction_context(&ctx, data_path, log_path);
    PNUM page_number = INVALID_PNUM;

    struct table_info people_info = { 0 };

    init_table_info(&people_info);

    add_char(&people_info, "name", 4, 15);
    add_int(&people_info, "age", 3);

    int written = 0;
    struct transaction *write_tsx = begin_transaction(&ctx);
    {
        struct heap_table people = { 0 };

        create_heap_table(&people, &people_info, write_tsx);

        for (int i = 0; i < (23 * 70); i++) //
        {
            struct record_id rid = heap_insert(&people);

            set_char(&people, rid, "name", "Heather Portaro");
            set_int(&people, rid, "age", i + 1);

            written++;
        }
        
        commit(write_tsx);
    }

    int read = 0;
    struct transaction *read_tsx = begin_transaction(&ctx);
    {
        struct heap_table read_table;
        struct heap_iterator iterator;

        open_heap_table(&read_table, &people_info, read_tsx, 0);
        open_heap_iterator(&read_table, &iterator);

        int age = 0;
        char name[16] = { 0 };

        struct value v = { 0 };

        while (next_record(&iterator))
        {
            struct record_id current_rid = iterator.current_record;
            
            for (int i = 0; i < read_table.table_info->column_count; i++)
            {
                get_value(&read_table, current_rid, i, &v);
            }
           
            read++;
        }
        
        reset(&v);

        commit(read_tsx);
    }

    assert(written == read);

    free_transaction_context(&ctx);
}

int main(void)
{
    single_table_page_write_read();
    complete_table();
    read_table_with_values();

    return EXIT_SUCCESS;

}