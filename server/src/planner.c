#include <planner.h>

struct plan * 
create_select_plan(struct select *select, struct query_ctx *ctx);

struct plan * 
create_plan(struct sql_stmt *sql, struct query_ctx *ctx)
{


    struct plan *plan = NULL;
    switch(sql->type)
    {
        case STMT_SELECT:
            plan = create_select_plan((struct select *)sql, ctx);
            break;
        default:
            break;
    }

    return plan;
}

struct plan * 
create_select_plan(struct select *select, struct query_ctx *ctx)
{
    struct plan *plan = NULL;
    // Handle not data special case
    if (select->table_refs == NULL)
    {
        plan = new_no_data_select_plan(select);
    }
    else
    {
        plan = new_select_stmt_plan(select, ctx);
    }

    return plan;
}
