#ifndef __HEAP_TABLE_H__
#define __HEAP_TABLE_H__

struct table_info
{
    char *table_name;
    uint16_t record_size;

    struct column columns[MAX_COLUMNS];
    uint16_t column_count;
};


void new_heap_table(struct table *table, struct table_info *table_info, struct transaction *tsx);


#endif