#include <buffers.h>
#include <storage.h>
#include <vector.h>

#include <string.h>


void 
init_table_info(struct table_info *table)
{
    table->record_size = 0;
    table->column_count = 0;
}

void 
add_int(struct table_info *table, const char *name, const uint16_t name_length)
{
    memcpy(table->columns[table->column_count].name, name, name_length);

    table->columns[table->column_count].size = sizeof(int);
    table->columns[table->column_count].type = TYPE_INT;
    table->columns[table->column_count].offset = table->record_size;

    table->column_count++;
    table->record_size += sizeof(int);
}

void 
add_char(struct table_info *table, const char *name, const uint16_t name_length, uint16_t size)
{
    memcpy(table->columns[table->column_count].name, name, name_length);

    table->columns[table->column_count].size = size;
    table->columns[table->column_count].type   = TYPE_CHAR;
    table->columns[table->column_count].offset = table->record_size;
    
    table->column_count++;
    table->record_size += size;
}

void free_table_info(struct table_info *table)
{
    for (int i = 0; i < table->column_count; i++)
    {
        free(table->columns[i].name);
    }
}
