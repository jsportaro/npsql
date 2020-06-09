#include <common.h>
#include <data_page.h>
#include <heap_table.h>
#include <storage.h>
#include <transaction.h>

#include <stdbool.h>
#include <stdint.h>

#define ALLOCAITON_MAP_SIZE (sizeof(struct allocation_map_header))
#define AM_ENTRY_LIMIT ((PAGE_SIZE - ALLOCAITON_MAP_SIZE) / sizeof(PNUM))

static bool 
add_new_data_page(struct heap_table *table, PNUM allocation_map_pid);

static PNUM 
add_new_am(struct heap_table *table, struct allocation_map_header *preceding);

static struct am_scan_result 
scan_am_for_insert(struct heap_table *table, PNUM am_pid, uint16_t start_at);

void 
create_heap_table(struct heap_table *table, struct table_info *table_info, struct transaction *tsx)
{
    table->tsx = tsx;
    table->table_info = table_info;
    table->am_entry_pid = add_new_am(table, NULL);

    add_new_data_page(table, table->am_entry_pid);
}

void 
open_heap_table(struct heap_table *table, struct table_info *table_info, struct transaction *tsx, PNUM first_am)
{
    table->tsx = tsx;
    table->table_info = table_info;
    table->am_entry_pid = first_am;
}

struct record_id 
heap_insert(struct heap_table *table)
{
    struct am_scan_result result = { 0 };
    uint16_t am_scan_staring_index = 0;
    uint32_t i = 0;
    PNUM current_am_pid = table->am_entry_pid;
    UNUSED(i);

am_scan_here_i_am:
    i++;
    result = scan_am_for_insert(table, current_am_pid, am_scan_staring_index);

    uint16_t a = INVALID_RECORD_HANDLE;
    if (result.handle == a)
    {
        // We went through the entire AM and didn't find a free page.
        // The following possibilities exist:
        // A)  This AM isn't full:
        //     1) Create a new table page
        //     2) Add to AM and alloc record
        // B)  This AM is full and has a valid next AM
        //     1) Get next AM
        //     2) Start looking through it's pages for one with free space
        // C)  This AM is full and does not have a vlid next AM
        //     1) Create new AM
        //     2) Link current AM to new AM
        //     3) Create a new table page
        //     4) Add to AM and alloc record


        // CASE A
        uint16_t limit = AM_ENTRY_LIMIT;
        if (result.am_entry_index < limit)
        {
            add_new_data_page(table, current_am_pid);
            am_scan_staring_index = result.am_entry_index;

            goto am_scan_here_i_am;
        }

        // CASE B
        if (result.am.next != INVALID_PNUM)
        {
            current_am_pid = result.am.next;
            am_scan_staring_index = 0;

            goto am_scan_here_i_am;
        }

        // CASE C
        if (result.am.next == INVALID_PNUM)
        {
            current_am_pid = add_new_am(table, &result.am);
            add_new_data_page(table, current_am_pid);
            am_scan_staring_index = 0;

            goto am_scan_here_i_am;
        }

        // CASE D Something is wrong - I missed somethings - I must die
        assert(false);
    }

    return result.rid;
}

bool open_heap_iterator(struct heap_table *table, struct heap_iterator *iterator)
{
    PNUM first_table_page_pid = INVALID_PNUM;
    
    iterator->table = table;
    iterator->current_am_pid_index = 0;

    transactional_pin(table->tsx, table->am_entry_pid);
    transactional_read(table->tsx, table->am_entry_pid, &iterator->current_am, 0, ALLOCAITON_MAP_SIZE);

    if (iterator->current_am.pages_count == 0)
    {
        //  Edge case - table has no rows
        return false;
    }

    
    transactional_read(
        table->tsx, 
        iterator->current_am.me, 
        &first_table_page_pid, 
        (sizeof(PNUM) * iterator->current_am_pid_index) + ALLOCAITON_MAP_SIZE, 
        sizeof(PNUM));

    open_data_page(first_table_page_pid, &iterator->current_page, iterator->table->table_info, iterator->table->tsx);
    open_page_iterator(&iterator->current_page, &iterator->current_page_iterator);

    return true;
}

bool next_record(struct heap_iterator *iterator)
{
    RECORD_HANDLE handle = INVALID_RECORD_HANDLE;
    // 1) Attempt to get record from current page iterator
    // 2) If not, try to get next page in am open iterator go to 1,
    // 3) if not, go to next am and go to 1
attempt_to_get_next:
    handle = next_page_record(&iterator->current_page_iterator);

    if (handle != INVALID_RECORD_HANDLE)
    {
        iterator->current_record.pid = iterator->current_page.page_number;
        iterator->current_record.slot = iterator->current_page_iterator.current_slot - 1;

        return true;
    }
    else
    {
        if (iterator->current_am_pid_index == iterator->current_am.pages_count - 1)
        {
            if (iterator->current_am.next == INVALID_PNUM)
            {
                //  end of the line
                return false;
            }
            else
            {
                PNUM old_am = iterator->current_am.me;
                iterator->current_am_pid_index = 0;

                transactional_pin(iterator->table->tsx, iterator->current_am.next);
                transactional_read(
                    iterator->table->tsx, 
                    iterator->current_am.next, 
                    &iterator->current_am, 
                    HEADER_OFFSET, 
                    ALLOCAITON_MAP_SIZE);

                transactional_unpin(iterator->table->tsx, old_am);
            }
        }
        else
        {
            iterator->current_am_pid_index++;
        }

        transactional_unpin(iterator->table->tsx, iterator->current_page.page_number);
        PNUM next_table_page_pid;

        transactional_read(
                iterator->table->tsx, 
                iterator->current_am.me, 
                &next_table_page_pid, 
                (sizeof(PNUM) * iterator->current_am_pid_index) + ALLOCAITON_MAP_SIZE, 
                sizeof(PNUM));

        open_data_page(next_table_page_pid, &iterator->current_page, iterator->table->table_info, iterator->table->tsx);
        open_page_iterator(&iterator->current_page, &iterator->current_page_iterator);

        goto attempt_to_get_next;
    }
}

void get_int(struct heap_table *table, struct record_id rid, const char *column, int32_t *value)
{
    struct data_page page;
    open_data_page(rid.pid, &page, table->table_info, table->tsx);
    page_get_int(&page, rid.slot, column, value);
}

void get_char(struct heap_table *table, struct record_id rid, const char *column, char *value)
{
    struct data_page page;
    open_data_page(rid.pid, &page, table->table_info, table->tsx);
    page_get_char(&page, rid.slot, column, value);
}

void set_int(struct heap_table *table, struct record_id rid, const char *column, int value)
{
    struct data_page page;
    open_data_page(rid.pid, &page, table->table_info, table->tsx);
    page_set_int(&page, rid.slot, column, value);
}

void set_char(struct heap_table *table, struct record_id rid, const char *column, char *value)
{
    struct data_page page;
    open_data_page(rid.pid, &page, table->table_info, table->tsx);
    page_set_char(&page, rid.slot, column, value);
}

static PNUM 
add_new_am(struct heap_table *table, struct allocation_map_header *preceding)
{
    struct allocation_map_header new_am = { 0 };

    PNUM new_am_pid = transactional_append(table->tsx);

    new_am.me = new_am_pid;
    new_am.next = INVALID_PNUM;
    new_am.page_type = AM_PAGE_TYPE;
    new_am.pages_count = 0;

    transactional_update(table->tsx, new_am_pid, &new_am, 0, ALLOCAITON_MAP_SIZE);

    if (preceding != NULL)
    {
        preceding->next = new_am_pid;

        transactional_update(table->tsx, preceding->me, preceding, 0, ALLOCAITON_MAP_SIZE);
    }

    return new_am_pid;
}

static bool 
add_new_data_page(struct heap_table *table, PNUM allocation_map_pid)
{
    struct data_page new_page;
    struct allocation_map_header am;

    PNUM new_table_pid = new_data_page(&new_page, table->table_info, table->tsx);
    
    transactional_pin(table->tsx, allocation_map_pid);
    transactional_read(table->tsx, allocation_map_pid, &am, 0, ALLOCAITON_MAP_SIZE);

    if (am.pages_count == AM_ENTRY_LIMIT)
    {
        return false;
    }

    transactional_update(table->tsx, am.me, &new_table_pid, ALLOCAITON_MAP_SIZE + (sizeof(PNUM) * am.pages_count), sizeof(PNUM));
    am.pages_count++;
    transactional_update(table->tsx, am.me, &am, 0, ALLOCAITON_MAP_SIZE);

    return true;
}

static struct am_scan_result 
scan_am_for_insert(struct heap_table *table, PNUM am_pid, uint16_t start_at)
{
    struct am_scan_result r = { 0 };
    struct data_page page= { 0 };
    PNUM table_page_pid = INVALID_PNUM;
    RECORD_HANDLE handle = INVALID_RECORD_HANDLE;

    r.am_entry_index = start_at;
    r.handle = INVALID_RECORD_HANDLE;
    transactional_pin(table->tsx, am_pid);
    transactional_read(table->tsx, am_pid, &r.am, 0, ALLOCAITON_MAP_SIZE);

    while (r.am_entry_index < r.am.pages_count)
    {
        transactional_read(table->tsx, r.am.me, &table_page_pid, (sizeof(PNUM) * r.am_entry_index) + ALLOCAITON_MAP_SIZE, sizeof(PNUM));

        open_data_page(table_page_pid, &page, table->table_info, table->tsx);

        handle = data_page_insert(&page);

        if (handle != INVALID_RECORD_HANDLE)
        {
            r.handle = handle;
            r.rid.slot = page.header.slot_count - 1;
            r.rid.pid = page.page_number;

            break;
        }       
        
        r.am_entry_index++;
    }

    return r;
}
