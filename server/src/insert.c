#include <common.h>
#include <insert.h>
#include <syscat.h>

bool execute_insert_into(struct transaction *tsx, struct syscat *cat, struct insert *i)
{
    struct table_info ti;
    UNUSED(cat);
    UNUSED(i);
    fetch_table_info(i->name, &ti, cat, tsx);
    UNUSED(ti);

    //  Get table's table_info structure
    //  make sure the columns named and the value expressions match up
    //  open table iterator
    //  create new row
    //  copy values

    return false;
}