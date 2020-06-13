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
    struct plan *plan = new_select_stmt_plan(select, ctx);

    vector_type(struct plan_column) columns = NULL;

    for(size_t i = 0; i < vector_size(select->expr_ctx_list); i++)
    {
        for (size_t j = 0; j < vector_size(select->expr_ctx_list[i]->unresolved); j++)
        {
            struct plan_column c;
            bool found = plan->get_column(plan, select->expr_ctx_list[i]->unresolved[j], &c);

            if (found)
            {
                vector_push(columns, c);
            }
            else
            {
                vector_free(columns);
                return NULL;
            }
        }

        //  Now, Need to see what type it'll be

    }

    return plan;
}
