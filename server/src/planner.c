#include <planner.h>

struct plan * 
create_select_plan(struct select *select, struct syscat *cat, struct transaction *tsx);

struct plan * 
create_plan(struct sql_stmt *sql, struct syscat *cat, struct transaction *tsx)
{


    struct plan *plan = NULL;
    switch(sql->type)
    {
        case STMT_SELECT:
            plan = create_select_plan((struct select *)sql, cat, tsx);
            break;
        default:
            break;
    }

    return plan;
}

struct plan * 
create_select_plan(struct select *select, struct syscat *cat, struct transaction *tsx)
{
    struct plan *plan = NULL;

    // Handle not data special case
    if (select->table_refs == NULL)
    {
        plan = new_no_data_select_plan(select);
    }
    else
    {
        plan = new_select_stmt_plan(select, cat, tsx);
    }

    return plan;
}
