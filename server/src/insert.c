#include <common.h>
#include <insert.h>
#include <heap_table.h>
#include <syscat.h>
#include <sql.h>
#include <vector.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool execute_insert_into(struct transaction *tsx, struct syscat *cat, struct insert *i)
{
    // How it works 
    // The code assumes that the given struct insert is valid
    // First, it tries to grab the struct table_info from the syscat.
    // If that's okay, it'll creat a new row and start to insert
    // values.  If types don't match, then it'll return false.  
    // Calling code will need to do the rollback.
    struct table_info ti;
    PNUM first_am;

    if (fetch_table_info(i->name, &ti, &first_am, cat, tsx) == false)
    {
        return false;
    }

    struct heap_table t = { 0 };
    open_heap_table(&t, &ti, tsx, first_am);
    struct record_id rid = heap_insert(&t);
    bool column_match = false;
    for (size_t x = 0; x < vector_size(i->columns); x++)
    {
        column_match = false;
        for (int j = 0; j < ti.column_count; j++)
        {
            if (strncmp(i->columns[x], ti.columns[j].name, strlen(i->columns[x])) == 0)
            {
                column_match = true;

                switch (ti.columns[j].type)
                {
                    case TYPE_INT:
                        set_int(&t, rid, ti.columns[j].name, ((struct term_expr *)i->values[x])->value.number);
                        break;
                    case TYPE_CHAR:
                        set_char(&t, rid, ti.columns[j].name, ((struct term_expr *)i->values[x])->value.string);
                        break;
                }
            }
        }

        if (column_match != true)
        {
            break;
        }
    }


    return column_match;
}
