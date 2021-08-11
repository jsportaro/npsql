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
lookup(vector_type(struct plan_column *) schema, struct identifier *n)
{
    for (size_t i = 0; i < vector_size(schema); i++)
    {
        if (strcmp(schema[i]->name, n->name) == 0)
        {
            if (n->qualifier != NULL)
            {
                if (strcmp(schema[i]->table, n->qualifier) == 0)
                {
                    return schema[i];
                }
            }
            else
            {
                return schema[i];
            }
        }
    }
    
    return NULL;
}

struct plan_column
resolve_type(struct expr *e, vector_type(struct plan_column *) schema)
{
    struct term_expr *term;
    struct infix_expr *infix = NULL;
    struct plan_column pr = { 0 };

    switch (e->type)
    {
        case EXPR_IDENIFIER:
            term = (struct term_expr *)e;
            struct plan_column *found = lookup(schema, term->value.identifier);

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
                struct plan_column tl = resolve_type(infix->l, schema);
                struct plan_column tr = resolve_type(infix->r, schema);

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
                struct plan_column tl = resolve_type(infix->l, schema);
                struct plan_column tr = resolve_type(infix->r, schema);

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
                struct plan_column tl = resolve_type(infix->l, schema);
                struct plan_column tr = resolve_type(infix->r, schema);

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
    
    vector_type(struct plan_column *) schema = plan->get_columns(plan);

    size_t exprs = vector_size(select->expr_ctx_list);

    // Determine column type and type safety
    for(size_t i = 0; i < exprs; i++)
    {
        if (select->expr_ctx_list[i]->expr != NULL)
        {
            struct plan_column pc = resolve_type(select->expr_ctx_list[i]->expr, schema);
            
            if (pc.type == TYPE_UNKNOWN)
            {
                result->status = PLANNER_ERROR;
                goto end;
            }
            
            // if (select->expr_ctx_list[i]->expr->type == EXPR_IDENIFIER)
            // {
            //     column.name = ((struct term_expr *)select->expr_ctx_list[i]->expr)->value.string;
            // }
            // else
            // {
                column.name = select->expr_ctx_list[i]->col_name;
            // }

            column.type = pc.type;
            column.size = pc.size;
            column.expr = select->expr_ctx_list[i]->expr;

            vector_push(result->columns, column);
        }
        else
        {
            for (size_t i = 0; i < vector_size(schema); i++)
            {
                vector_push(result->columns, *schema[i]);
            }
        }
    }

    // Ensure where condense to bool
    if (select->where != NULL)
    {
        struct plan_column where_type = resolve_type(select->where, schema);
        if (where_type.type != TYPE_BOOL)
        {
            result->status = PLANNER_ERROR;
            goto end;
        }
    }
end:
    return result;
}
