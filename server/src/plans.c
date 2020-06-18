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

static struct scan * 
open_project_scan(struct plan *plan)
{
    struct project_plan *pp = (struct project_plan *)plan;
    
    if (pp->p == NULL)
    {
        return new_project_scan(NULL);
    }
    else
    {
        return new_project_scan(pp->p->open(pp->p));
    }
}

bool project_plan_get_column(struct plan *plan, char *name, struct plan_column *column)
{
    struct project_plan *pp = (struct project_plan *)plan;

    return pp->p->get_column(pp->p, name, column);
}

vector_type(struct plan_column *) project_plan_get_columns(struct plan *plan)
{
    struct project_plan *pp = (struct project_plan *)plan;

    return pp->p->get_columns(pp->p);
}

struct plan *
new_project_plan(struct plan *p)
{
    struct project_plan *pp = malloc(sizeof(struct project_plan));
    assert(pp != NULL);
    pp->type = PROJECT_PLAN;
    pp->p = p;
    pp->open = &open_project_scan;
    pp->get_column = &project_plan_get_column;
    pp->get_columns = &project_plan_get_columns;

    return (struct plan *)pp;
}

static struct scan * 
open_select_scan(struct plan *plan)
{
    struct select_plan *sp = (struct select_plan *)plan;
    
    return new_select_scan(sp->p->open(sp->p), sp->where_clause);
}

bool select_scan_get_column(struct plan *plan, char *name, struct plan_column *column)
{
    struct select_plan *sp = (struct select_plan *)plan;

    return sp->p->get_column(sp->p, name, column);
}

vector_type(struct plan_column *) select_plan_get_columns(struct plan *plan)
{
    struct select_plan *sp = (struct select_plan *)plan;

    return sp->p->get_columns(sp->p);
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
    sp->get_column = &select_scan_get_column;
    sp->get_columns = &select_plan_get_columns;

    return (struct plan *)sp;
}

struct scan *
open_product_scan(struct plan *plan)
{
    struct product_plan *pp = (struct product_plan *)plan;

    return new_product_scan(pp->l->open(pp->l), pp->r->open(pp->r));
}

bool product_scan_get_column(struct plan *plan, char *name, struct plan_column *column)
{
    struct product_plan *pp = (struct product_plan *)plan;

    if (pp->l->get_column(pp->l, name, column) == true)
    {
        return true;
    }

    if (pp->r->get_column(pp->r, name, column) == true)
    {
        return true;
    }

    return false;
}

vector_type(struct plan_column *)
product_scan_get_columns(struct plan *plan)
{
    struct product_plan *pp = (struct product_plan *)plan;

    vector_type(struct plan_column *) lpc = pp->l->get_columns(pp->l);
    vector_type(struct plan_column *) rpc = pp->r->get_columns(pp->r);

    for (size_t i = 0; i < vector_size(lpc); i++)
    {
        vector_push(pp->ppc, lpc[i]);
    }

    for (size_t i = 0; i < vector_size(rpc); i++)
    {
        vector_push(pp->ppc, rpc[i]);
    }

    return pp->ppc;
}

struct plan *
new_product_plan(struct plan *l, struct plan *r)
{
    struct product_plan *pp = malloc(sizeof(struct product_plan));
    assert(pp != NULL);
    pp->type = PRODUCT_PLAN;
    pp->r = r;
    pp->l = l;
    pp->ppc = NULL;
    pp->open = &open_product_scan;
    pp->get_column = &product_scan_get_column;
    pp->get_columns = &product_scan_get_columns;

    return (struct plan *)pp;
}

struct scan *
open_table_scan(struct plan *plan)
{
    struct table_plan *tp = (struct table_plan *)plan;

    return new_table_scan(&tp->ti, tp->first_am, tp->ctx);   
}

bool table_scan_get_column(struct plan *plan, char *name, struct plan_column *column)
{
    struct table_plan *tp = (struct table_plan *)plan;

    for (size_t i = 0; i < tp->ti.column_count; i++)
    {
        if (strcmp(tp->ti.columns[i].name, name) == 0)
        {
            column->name = tp->ti.columns[i].name;
            column->size = tp->ti.columns[i].size;
            column->type = tp->ti.columns[i].type;

            return true;
        }
    }

    return false;
}

vector_type(struct plan_column *) 
table_plan_get_columns(struct plan *plan)
{
    struct table_plan *tp = (struct table_plan *)plan;

    for (size_t i = 0; i < tp->ti.column_count; i++)
    {
        struct plan_column *pc = malloc(sizeof(struct plan_column));

        pc->name = tp->ti.columns[i].name;
        pc->size = tp->ti.columns[i].size;
        pc->type = tp->ti.columns[i].type;
        pc->expr = NULL;
        
        vector_push(tp->columns, pc);
    }

    return tp->columns;
}

struct plan *
new_table_plan(struct table_ref *table_ref, struct query_ctx *ctx)
{
    struct table_plan *tp = malloc(sizeof(struct table_plan));
    assert(tp != NULL);
    memset(tp, 0, sizeof(struct table_plan));
    
    tp->type = TABLE_PLAN;
    tp->open = &open_table_scan;
    tp->get_column = &table_scan_get_column;
    tp->get_columns = &table_plan_get_columns;

    tp->ctx = ctx;
    tp->ti.column_count = 0;
    tp->ti.record_size = 0;
   
    tp->columns = NULL;
    tp->first_am = INVALID_PNUM;

    fetch_table_info(table_ref->table_name, &tp->ti, &tp->first_am, ctx->cat, ctx->tsx);

    return (struct plan *)tp;
}

struct plan * 
new_select_stmt_plan(struct select *select, struct query_ctx *ctx)
{
    vector_type(struct plan *) table_plans = NULL;
    struct plan *p = NULL;
    for (size_t i = 0; i < vector_size(select->table_refs); i++)
    {
        struct plan *tp = new_table_plan(select->table_refs[i], ctx);

        vector_push(table_plans, tp);
    }

    if (vector_size(table_plans) > 0)
    {
        p = table_plans[0];
        for (size_t i = 1; i < vector_size(table_plans); i++)
        {
            p = new_product_plan(p, table_plans[i]);
        }

        if (select->where != NULL)
        {
            p = new_select_plan(p, select->where);
        }
    }

    p = new_project_plan(p);
    vector_free(table_plans);

    return p;
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
            vector_free(product->ppc);
            free(product);
            product = NULL;
            break;
        case TABLE_PLAN:
            table = (struct table_plan *)p;
            for (size_t i = 0; i < vector_size(table->columns); i++)
            {
                free(table->columns[i]);
            }
            vector_free(table->columns);
            free(table);
            table = NULL;
            break;
        default:
            
            break;
    }
}
