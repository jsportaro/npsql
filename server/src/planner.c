#include <planner.h>

struct planner_result *
create_select_plan(struct select *select, struct query_ctx *ctx);

struct planner_result *
create_plan(struct sql_stmt *sql, struct query_ctx *ctx)
{
    struct planner_result *plan = NULL;
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

struct plan_column *
lookup(vector_type(struct plan_column) c, char *n)
{
    for (size_t i = 0; i < vector_size(c); i++)
    {
        if (strcmp(c[i].name, n) == 0)
        {
            return &c[i];
        }
    }
    
    return NULL;
}

struct plan_column
resolve_type(struct expr *e, vector_type(struct plan_column) c)
{
    struct term_expr *term;
    struct infix_expr *infix = NULL;
    struct plan_column pr = { 0 };

    switch (e->type)
    {
        case EXPR_IDENIFIER:
            term = (struct term_expr *)e;
            struct plan_column *found = lookup(c, term->value.string);

            if (found != NULL)
            {   
                pr.name = found->name;
                pr.size = found->size;
                pr.type = found->type;
            }

            break;
        case EXPR_INTEGER:
            pr.type = TYPE_INT;
            break;
        case EXPR_STRING:
            pr.type = TYPE_CHAR;
            break;
        case EXPR_ADD:
        case EXPR_SUB:
        case EXPR_MUL:
        case EXPR_DIV: {
                infix = (struct infix_expr *)e;
                struct plan_column tl = resolve_type(infix->l, c);
                struct plan_column tr = resolve_type(infix->r, c);

                if (tl.type == tr.type)
                {
                    pr.type = tl.type;
                }
                else
                {
                    pr.type = TYPE_UNKNOWN;
                }
            }
            break;
        case EXPR_EQU: {
                infix = (struct infix_expr *)e;
                struct plan_column tl = resolve_type(infix->l, c);
                struct plan_column tr = resolve_type(infix->r, c);

                if (tl.type == tr.type)
                {
                    pr.type = TYPE_BOOL;
                }
                else
                {
                    pr.type = TYPE_UNKNOWN;
                }
            }
            break;
        case EXPR_AND:
        case EXPR_OR:  {
                infix = (struct infix_expr *)e;
                struct plan_column tl = resolve_type(infix->l, c);
                struct plan_column tr = resolve_type(infix->r, c);

                if (tl.type == TYPE_BOOL && tr.type == TYPE_BOOL)
                {
                    pr.type = TYPE_BOOL;
                }
                else
                {
                    pr.type = TYPE_UNKNOWN;
                }
            }
            break;
        default:
            break;
    }

    return pr;
}

struct planner_result *
create_select_plan(struct select *select, struct query_ctx *ctx)
{
    struct planner_result *result = malloc(sizeof(struct planner_result));
    struct plan *plan = new_select_stmt_plan(select, ctx);
    struct plan_column column = { 0 };

    result->plan    = plan;
    result->columns = NULL;
    result->status  = PLANNER_SUCCESS;

    vector_type(struct plan_column) resolved = NULL;

    // Resolve references
    for (size_t i = 0; i < vector_size(select->unresolved); i++)
    {
        struct plan_column c = { 0 };
        bool found = plan->get_column(plan, select->unresolved[i], &c);

        if (found == true)
        {
            vector_push(resolved, c);
        }
        else
        {
            vector_free(resolved);
            result->status = PLANNER_ERROR;
            goto end;
        }
    }

    // Determine column type and type safety
    for(size_t i = 0; i < vector_size(select->expr_ctx_list); i++)
    {
        struct plan_column pc = resolve_type(select->expr_ctx_list[i]->expr, resolved);
        
        if (pc.type == TYPE_UNKNOWN)
        {
            result->status = PLANNER_ERROR;
            goto end;
        }
        
        if (select->expr_ctx_list[i]->expr->type == EXPR_IDENIFIER)
        {
            column.name = ((struct term_expr *)select->expr_ctx_list[i]->expr)->value.string;
        }
        else
        {
            column.name = select->expr_ctx_list[i]->col_name;
        }
        column.type = pc.type;
        column.size = pc.size;
        
        vector_push(result->columns, column);
    }

    // Ensure where condenses to bool
    if (select->where != NULL)
    {
        struct plan_column where_type = resolve_type(select->where, resolved);
        if (where_type.type != TYPE_BOOL)
        {
            result->status = PLANNER_ERROR;
            goto end;
        }
    }
end:
    vector_free(resolved);
    return result;
}
