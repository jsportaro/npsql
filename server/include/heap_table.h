#ifndef __HEAP_TABLE_H__
#define __HEAP_TABLE_H__

#include <data_file.h>
#include <storage.h>
#include <transaction.h>

#include <stdbool.h>
#include <stdint.h>

//  This unfortunately burns 4 bytes to padding on
//  64bit
struct allocation_map_header
{
    uint16_t page_type;
    uint16_t pages_count;

    PNUM me;
    PNUM next;
};

struct am_scan_result
{
    RECORD_HANDLE handle;
    struct record_id rid;
    uint16_t am_entry_index;
    struct allocation_map_header am;
};

struct heap_table
{
    struct transaction *tsx;
    struct table_info *table_info;
    PNUM am_entry_pid;
};

struct heap_iterator
{
    struct heap_table *table;

    struct record_id current_record;

    uint16_t current_am_pid_index;
    struct allocation_map_header current_am;

    struct data_page current_page;
    struct data_page_iterator current_page_iterator;
};

void create_heap_table(struct heap_table *table, struct table_info *table_info, struct transaction *tsx);
void open_heap_table(struct heap_table *table, struct table_info *table_info, struct transaction *tsx, PNUM first_am);
struct record_id heap_insert(struct heap_table *table);

void get_int(struct heap_table *table, struct record_id rid, const char *column, int32_t *value);
void get_char(struct heap_table *table, struct record_id rid, const char *column, char *value);
void set_int(struct heap_table *table, struct record_id rid, const char *column, int value);
void set_char(struct heap_table *table, struct record_id rid, const char *column, char *value);

bool open_heap_iterator(struct heap_table *table, struct heap_iterator *iterator);
bool next_record(struct heap_iterator *iterator);

#endif
