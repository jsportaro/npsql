#include <planner.h>

struct plan * create_select_plan(struct select *select);

struct plan * 
create_plan(struct sql_stmt *sql)
{
    struct plan *plan = NULL;
    switch(sql->type)
    {
        case STMT_SELECT:
            plan = create_select_plan((struct select *)sql);
            break;
        default:
            break;
    }

    return plan;
}

struct plan * create_select_plan(struct select *select)
{
    struct plan *plan = NULL;

    // Handle not data special case
    if (select->table_refs == NULL)
    {
        plan = new_no_data_select_plan(select);
    }

    return plan;
}
