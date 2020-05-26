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

struct plan * create_no_data_select_plan(struct select *select)
{
    struct project *p = malloc(sizeof(struct project));

    p->type = PLAN_PROJECT;
    p->expr_list = select->expr_list;

    return (struct plan *)p;
}

struct plan * create_select_plan(struct select *select)
{
    struct plan *plan = NULL;

    // Handle not data special case
    if (select->table_refs == NULL)
    {
        plan = create_no_data_select_plan(struct select *select);
    }

    return plan;
}
