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

enum npsql_type 
lookup(vector_type(struct plan_column) c, char *n)
{
    for (size_t i = 0; i < vector_size(c); i++)
    {
        if (strcmp(c[i].name, n) == 0)
        {
            return c[i].type;
        }
    }
    
    return TYPE_UNKNOWN;
}

enum npsql_type 
resolve_type(struct expr *e, vector_type(struct plan_column) c)
{
    struct term_expr *term;
    struct infix_expr *infix = NULL;

    switch (e->type)
    {
        case EXPR_IDENIFIER:
            term = (struct term_expr *)e;
            return lookup(c, term->value.string);
        case EXPR_INTEGER:
            return TYPE_INT;
        case EXPR_STRING:
            return TYPE_CHAR;
        case EXPR_ADD:
        case EXPR_SUB:
        case EXPR_MUL:
        case EXPR_DIV:
            infix = (struct infix_expr *)e;
            enum npsql_type tl = resolve_type(infix->l, c);
            enum npsql_type tr = resolve_type(infix->r, c);

            if (tl == tr)
            {
                return tl;
            }
            else
            {
                return TYPE_UNKNOWN;
            }

            break;
        default:
            return TYPE_UNKNOWN;
    }
}

struct planner_result *
create_select_plan(struct select *select, struct query_ctx *ctx)
{
    struct planner_result *result = malloc(sizeof(struct planner_result));
    struct plan *plan = new_select_stmt_plan(select, ctx);

    result->plan = plan;
    result->columns = NULL;

    for(size_t i = 0; i < vector_size(select->expr_ctx_list); i++)
    {
        for (size_t j = 0; j < vector_size(select->expr_ctx_list[i]->unresolved); j++)
        {
            struct plan_column c;
            bool found = plan->get_column(plan, select->expr_ctx_list[i]->unresolved[j], &c);

            if (found == true)
            {
                vector_push(result->columns, c);
            }
            else
            {
                vector_free(result->columns);
                return NULL;
            }
        }

        enum npsql_type t = resolve_type(select->expr_ctx_list[i]->expr, result->columns);
        
        if (t == TYPE_UNKNOWN)
        {
            return NULL;
        }
        select->expr_ctx_list[i]->type = t;

    }

    return result;
}
