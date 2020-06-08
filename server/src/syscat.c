#include <common.h>
#include <heap_table.h>
#include <storage.h>
#include <syscat.h>
#include <transaction.h>

#define OBJECT_CATALOG_AM_PID 0
#define TABLE_CATALOG_AM_PID 2
#define COLUMN_CATALOG_AM_PID 4

enum object_type
{
    OBJ_TABLE = 1,
    OBJ_VIEW  = 2,
    OBJ_STATS = 3,
    OBJ_INDEX = 4
};

static const char 
    *name_col = "name", 
    *object_type = "object_type",
    *am_pid = "am_pid",
    *table_name = "table_name", 
    *type = "type", 
    *length_col = "length", 
    *offset = "offset";

void 
create_system_catalogs(
    struct syscat *syscat, 
    struct transaction *tsx)
{
    intialize_system_catalogs(syscat);
    struct heap_table catalog_table;
    create_heap_table(&catalog_table, &syscat->object_catalog_schema, tsx);

    struct heap_table tables;
    create_heap_table(&tables, &syscat->table_catalog_schema, tsx);

    struct heap_table columns;
    create_heap_table(&columns, &syscat->column_catalog_schema, tsx);
}

void 
intialize_system_catalogs(struct syscat *syscat)
{
    init_table_info(&syscat->object_catalog_schema);
    add_char(&syscat->object_catalog_schema, name_col, (uint16_t)strlen(name_col), MAX_OBJECT_NAME);
    add_int(&syscat->object_catalog_schema, object_type, (uint16_t)strlen(object_type));

    init_table_info(&syscat->table_catalog_schema);
    add_char(&syscat->table_catalog_schema, name_col, (uint16_t)strlen(name_col), MAX_TABLE_NAME);
    add_int(&syscat->table_catalog_schema, am_pid, (uint16_t)strlen(am_pid));

    init_table_info(&syscat->column_catalog_schema);
    add_char(&syscat->column_catalog_schema, name_col, (uint16_t)strlen(name_col), MAX_COLUMN_NAME);
    add_char(&syscat->column_catalog_schema, table_name, (uint16_t)strlen(table_name), MAX_TABLE_NAME);
    add_int(&syscat->column_catalog_schema, type, (uint16_t)strlen(type));
    add_int(&syscat->column_catalog_schema, length_col, (uint16_t)strlen(length_col));
    add_int(&syscat->column_catalog_schema, offset, (uint16_t)strlen(offset));
}

void create_table(
    struct syscat *syscat,
    struct transaction *tsx, 
    struct table_info *schema)
{
    struct heap_table object_catalog;
    struct heap_table table_catalog;
    struct heap_table column_catalog;
    struct heap_table table;
    
    open_heap_table(&object_catalog, &syscat->object_catalog_schema, tsx, OBJECT_CATALOG_AM_PID);
    open_heap_table(&table_catalog, &syscat->table_catalog_schema, tsx, TABLE_CATALOG_AM_PID);
    open_heap_table(&column_catalog, &syscat->column_catalog_schema, tsx, COLUMN_CATALOG_AM_PID);

    struct record_id rid = heap_insert(&object_catalog);

    set_char(&object_catalog, rid, name_col, schema->table_name);
    set_int(&object_catalog, rid, object_type, OBJ_TABLE);

    create_heap_table(&table, schema, tsx);

    rid = heap_insert(&table_catalog);
    set_char(&table_catalog, rid, name_col, schema->table_name);
    set_int(&table_catalog, rid, am_pid, (int)table.am_entry_pid);

    for (int i = 0; i < schema->column_count; i++)
    {
        struct record_id col_rid = heap_insert(&column_catalog);

        struct column current = schema->columns[i];

        set_char(&column_catalog, col_rid, name_col, current.name);
        set_char(&column_catalog, col_rid, table_name, schema->table_name);
        set_int(&column_catalog, col_rid, type, current.type);
        set_int(&column_catalog, col_rid, length_col, current.size);
        set_int(&column_catalog, col_rid, offset, current.offset);
    }
}

bool
fetch_table_info(const char *name, struct table_info *ti, struct syscat *syscat, struct transaction *tsx)
{ 
    ti->column_count = 0;
    struct heap_table table_catalog;
    struct heap_table column_catalog;

    open_heap_table(&table_catalog, &syscat->table_catalog_schema, tsx, 0);
    struct heap_iterator iterator = { 0 };

    open_heap_iterator(&table_catalog, &iterator);
    int c_am_pid = 0;
    bool exists = false;
    while (next_record(&iterator))
    {
        struct record_id current_rid = iterator.current_record;
        char cname[MAX_TABLE_NAME];
        get_char(&table_catalog, current_rid, name_col, cname);

        if (strncmp(name, cname, MAX_COLUMN_NAME) == 0)
        {
            get_int(&table_catalog, current_rid, am_pid, &c_am_pid);
            exists = true;
        }

        break;
    }

    if (exists == true)
    {
        open_heap_iterator(&column_catalog, &iterator);

        while (next_record(&iterator))
        {
            struct record_id current_rid = iterator.current_record;

            char cname[MAX_TABLE_NAME];
            get_char(&table_catalog, current_rid, name_col, cname);
            if (strncmp(name, cname, MAX_COLUMN_NAME) == 0)
            {
                
            }
        }
    }

    return exists;
}