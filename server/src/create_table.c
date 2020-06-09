#include <common.h>
#include <create_table.h>

bool execute_create_table(struct transaction *tsx, struct syscat *cat, struct create_table *ct)
{
    struct table_info ti = { 0 };
    size_t name_length = strlen(ct->table_name);

    if (name_length > MAX_TABLE_NAME)
    {
        return false;
    }

    memcpy(ti.table_name, ct->table_name, name_length);
    uint16_t offset = 0;
    for (size_t i = 0; i < vector_size(ct->column_defs); i++)
    {
        size_t column_length = strlen(ct->column_defs[i]->name);

        if (column_length > MAX_COLUMN_NAME)
        {
            return false;
        }
        memcpy(ti.columns[i].name, ct->column_defs[i]->name, column_length);
        ti.columns[i].offset = offset;
        ti.columns[i].size =  ct->column_defs[i]->type->size;
        ti.columns[i].type = ct->column_defs[i]->type->type;

        offset += ti.columns[i].size;
        ti.column_count++;
    }

    create_table(cat, tsx, &ti);

    return true;
}
