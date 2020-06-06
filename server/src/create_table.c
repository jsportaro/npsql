#include <common.h>
#include <create_table.h>


bool execute_create_table(struct transaction *tsx, struct syscat *cat, struct create_table *ct)
{
    UNUSED(tsx);
    UNUSED(ct);
    UNUSED(cat);

    struct table_info ti = { 0 };

    ti.table_name = (char *)ct->table_name;
    uint16_t offset = 0;
    for (size_t i = 0; i < vector_size(ct->column_defs); i++)
    {
        ti.columns[i].name = (char *)ct->column_defs[i]->name;
        ti.columns[i].offset = offset;
        ti.columns[i].size =  ct->column_defs[i]->type->size;
        ti.columns[i].type = ct->column_defs[i]->type->type;

        offset += ti.columns[i].size;
        ti.column_count++;
    }

    create_table(cat, tsx, &ti);

    return true;
}
