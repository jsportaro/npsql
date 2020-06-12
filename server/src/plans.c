#include <assert.h>
#include <common.h>
#include <plans.h>
#include <query_context.h>
#include <scans.h>
#include <syscat.h>
#include <transaction.h>
#include <vector.h>

#include <stdlib.h>
#include <string.h>

static vector_type(struct column) 
bind_columns(struct select *select);

static struct scan * 
open_no_data_select_scan(struct plan *plan)
{
    struct project_plan *p = (struct project_plan *)plan;
    
    return new_scan_project(p->expr_list, NULL);
}

static struct scan * 
open_project_scan(struct plan *plan)
{
    struct project_plan *pp = (struct project_plan *)plan;
    
    return new_project_scan(pp->p->open(pp->p), pp->expr_list);

}

struct plan *
new_project_plan(struct plan *p, vector_type(struct expr_ctx *) expr_list)
{
    struct project_plan *pp = malloc(sizeof(struct project_plan));
    assert(pp != NULL);
    pp->type = PROJECT_PLAN;
    pp->p = p;
    pp->expr_list = expr_list;
    pp->open = &open_project_scan;

    return (struct plan *)pp;
}

static struct scan * 
open_select_scan(struct plan *plan)
{
    struct select_plan *sp = (struct select_plan *)plan;
    
    return new_select_scan(sp->p->open(sp->p), sp->where_clause);
}

struct plan *
new_select_plan(struct plan *p, struct expr *where_clause)
{
    struct select_plan *sp = malloc(sizeof(struct select_plan));
    assert(sp != NULL);
    sp->type = SELECT_PLAN;
    sp->p = p;
    sp->where_clause = where_clause;
    sp->open = &open_select_scan;

    return (struct plan *)sp;
}

struct scan *
open_product_scan(struct plan *plan)
{
    struct product_plan *pp = (struct product_plan *)plan;

    return new_product_scan(pp->l->open(pp->l), pp->r->open(pp->r));
}

struct plan *
new_product_plan(struct plan *l, struct plan *r)
{
    struct product_plan *pp = malloc(sizeof(struct product_plan));
    assert(pp != NULL);
    pp->type = PRODUCT_PLAN;
    pp->r = r;
    pp->l = l;
    pp->open = &open_project_scan;

    return (struct plan *)pp;
}

struct scan *
open_table_scan(struct plan *plan)
{
    struct table_plan *tp = (struct table_plan *)plan;

    return new_table_scan(&tp->ti, tp->first_am, tp->ctx);   
}

struct plan *
new_table_plan(struct table_ref *table_ref, struct query_ctx *ctx)
{
    struct table_plan *tp = malloc(sizeof(struct table_plan));
    assert(tp != NULL);
    
    tp->type = TABLE_PLAN;
    tp->open = &open_table_scan;
    tp->ctx = ctx;

    fetch_table_info(table_ref->table_name, &tp->ti, &tp->first_am, ctx->cat, ctx->tsx);

    return (struct plan *)tp;
}

struct plan * 
new_no_data_select_plan(struct select *select)
{
    struct project_plan *p = malloc(sizeof(struct project_plan));

    p->type = PROJECT_PLAN;
    p->expr_list = select->expr_ctx_list;
    p->column_list = bind_columns(select);
    p->open = &open_no_data_select_scan;

    return (struct plan *)p;
}

struct plan * 
new_select_stmt_plan(struct select *select, struct query_ctx *ctx)
{
    vector_type(struct plan *) table_plans = NULL;

    for (size_t i = 0; i < vector_size(select->table_refs); i++)
    {
        struct plan *tp = new_table_plan(select->table_refs[i], ctx);

        vector_push(table_plans, tp);
    }

    assert(vector_size(table_plans) > 0);

    struct plan *p = table_plans[0];
    for (size_t i = 1; i < vector_size(table_plans); i++)
    {
        p = new_product_plan(p, table_plans[i]);
    }

    if (select->where != NULL)
    {
        p = new_select_plan(p, select->where);
    }

    p = new_project_plan(p, select->expr_ctx_list);
    vector_free(table_plans);

    return p;
}

struct column
bind_column(struct expr *expr)
{
    struct term_expr *term;
    struct infix_expr *infix = NULL;
    UNUSED(term);
    struct column r;
    struct column lr, rr;

    r.size = 0;
    r.type   = TYPE_INT;
    
    switch(expr->type)
    {
        case EXPR_INTEGER:
            term = (struct term_expr *)expr;
            
            r.type = TYPE_INT;
            return r;
        case EXPR_IDENIFIER:
            term = (struct term_expr *)expr;
            
            //  Need to lookup metadata here
            
            return r;
        case EXPR_STRING:
            term = (struct term_expr *)expr;
            r.size = strlen(term->value.string) - 2;
            r.type = TYPE_CHAR;

            return r;
        case EXPR_ADD:
        case EXPR_SUB:
        case EXPR_MUL:
        case EXPR_DIV:
            infix = (struct infix_expr *)expr;
            lr = bind_column(infix->l);
            rr = bind_column(infix->r);
           
            // Type saftey!  Also, only ints for now
            if (lr.type != rr.type)
            {
                goto end;
            }

            r.type = lr.type;
            r.size = TYPE_INT_SIZE;
            break;
        case EXPR_COMPARISON:
            // error condition, select list shouldn't return
            // bools
            goto end;
            break;
    }
end:
    return r;
}

static vector_type(struct column) 
bind_columns(struct select *select)
{
    vector_type(struct column) pc = NULL;

    for (size_t i = 0; i < vector_size(select->expr_ctx_list); i++)
    {
        struct expr *expr = select->expr_ctx_list[i]->expr;
        struct column c = bind_column(expr);
        vector_push(pc, c);
    }

    return pc;
}

static void
free_project_plan(struct project_plan *p)
{
    // Note: do not free pp->expr_list
    //       it "belongs" to struct select
    vector_free(p->column_list);
}

void 
free_plan(struct plan *p)
{
    struct project_plan *project = NULL;
    struct select_plan  *select  = NULL;
    struct product_plan *product = NULL;
    struct table_plan   *table   = NULL;

    if (p == NULL)
    {
        return;
    }

    switch (p->type)
    {
        case PROJECT_PLAN:
            project = (struct project_plan *)p;
            free_project_plan(project);
            free_plan(project->p);
            free(project);
            project = NULL;
            break;
        case SELECT_PLAN:
            select = (struct select_plan *)p;
            free_plan(select->p);
            free(select);
            select = NULL;
            break;
        case PRODUCT_PLAN:
            product = (struct product_plan *)p;
            free_plan(product->l);
            free_plan(product->r);
            free(product);
            product = NULL;
            break;
        case TABLE_PLAN:
            table = (struct table_plan *)p;
            free(table);
            table = NULL;
            break;
        default:
            
            break;
    }
}
