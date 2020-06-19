#include <buffers.h>
#include <common.h>
#include <defaults.h>
#include <expr_eval.h>
#include <heap_table.h>
#include <query_context.h>
#include <scans.h>
#include <storage.h>

bool 
project_scan_next(struct scan *scan)
{
    struct project_scan *ps = (struct project_scan *)scan;

    if (ps->scan == NULL)
    {
        bool has_next = ps->has_next;

        if (ps->has_next == true)
        {
            ps->has_next = false;
        }

        return has_next;
    }
    
    return ps->scan->next(ps->scan);
}

void 
project_scan_get_value(struct scan *scan, struct identifier *column_name, struct value *value)
{
    struct project_scan *ps = (struct project_scan *)scan;
    
    ps->scan->get_value(ps->scan, column_name, value);
    
    return;
}

void
project_scan_reset(struct scan *scan)
{
    struct project_scan *ps = (struct project_scan *)scan;

    ps->scan->reset(ps->scan);
}

struct scan * 
new_project_scan(struct scan *inner)
{
    struct project_scan *ps = malloc(sizeof(struct project_scan));
    assert(ps != NULL);

    ps->type = PROJECT_SCAN;
    ps->next = &project_scan_next;
    ps->get_value = &project_scan_get_value;
    ps->reset = &project_scan_reset;

    ps->has_next = true;
    ps->scan = inner;

    return (struct scan *)ps;
}

static bool 
select_scan_next(struct scan *scan)
{
    struct select_scan *ss = (struct select_scan *)scan;

    while (ss->scan->next(ss->scan) == true)
    {
        struct value v = eval(ss->where_clause, ss->scan);

        if (v.type == TYPE_BOOL && v.as.boolean == true)
        {
            return true;
        }
    }
    
    return false;
}

static void 
select_scan_get_value(struct scan *scan, struct identifier *column_name, struct value *value)
{
    struct select_scan *ss = (struct select_scan *)scan;

    ss->scan->get_value(ss->scan, column_name, value);
}

void
select_scan_reset(struct scan *scan)
{
    struct select_scan *ss = (struct select_scan *)scan;

    ss->scan->reset(ss->scan);
}

struct scan * 
new_select_scan(struct scan *inner, struct expr *where_clause)
{
    struct select_scan *ss = malloc(sizeof(struct select_scan));
    assert(ss != NULL);

    ss->type = SELECT_SCAN;
    ss->where_clause = where_clause;
    ss->next = &select_scan_next;
    ss->get_value = &select_scan_get_value;
    ss->reset = &select_scan_reset;    
    ss->scan = inner;

    return (struct scan *)ss;
}

static bool 
product_scan_next(struct scan *scan)
{
    struct product_scan *ps = (struct product_scan *)scan;

    if (ps->primed != true)
    {
        return false;
    }

    if (ps->r->next(ps->r) == true)
    {
        return true;
    }
    else
    {
        ps->r->reset(ps->r);

        return ps->r->next(ps->r) && ps->l->next(ps->l);
    }
}

void 
product_scan_get_value(struct scan *scan, struct identifier *column_name, struct value *value)
{
    struct product_scan *ps = (struct product_scan *)scan;

    // Working under the assumption things can get resolved
    ps->l->get_value(ps->l, column_name, value);

    if (value->type == TYPE_UNKNOWN)
    {
        reset(value);
        ps->r->get_value(ps->r, column_name, value);
    }
}

static void
product_scan_reset(struct scan *scan)
{
    struct product_scan *ps = (struct product_scan *)scan;

    ps->l->reset(ps->l);
    ps->r->reset(ps->r); 
}

struct scan * 
new_product_scan(struct scan *l, struct scan *r)
{
    struct product_scan *ps = malloc(sizeof(struct product_scan));
    assert(ps != NULL);

    ps->type = PRODUCT_SCAN;
    ps->l = l;
    ps->r = r;
    ps->next = &product_scan_next;
    ps->get_value = &product_scan_get_value;
    ps->reset = &product_scan_reset;
    ps->primed = ps->l->next(ps->l);

    return (struct scan *)ps;
}

static bool 
table_scan_next(struct scan *scan)
{
    struct table_scan *ts = (struct table_scan *)scan;

    return next_record(&ts->i);
}

static void
table_scan_get_value(struct scan *scan, struct identifier *name, struct value *v)
{
    struct table_scan *ts = (struct table_scan *)scan;
    int i = 0;

    if (name->qualifier != NULL)
    {
        if (strncmp(name->qualifier, ts->ti->table_name, MAX_TABLE_NAME) != 0)
        {
            v->type = TYPE_UNKNOWN;

            return;
        }
    }
    for (; i < ts->ti->column_count; i++)
    {
        if (strncmp(name->name, ts->ti->columns[i].name, MAX_COLUMN_NAME) == 0)
        {
            break;
        }
    }

    get_value(&ts->ht, ts->i.current_record, i, v);
}

static void
table_scan_reset(struct scan *scan)
{
    struct table_scan *ts = (struct table_scan *)scan;

    open_heap_iterator(&ts->ht, &ts->i);
}

struct scan * 
new_table_scan(struct table_info *ti, PNUM first_am, struct query_ctx *ctx)
{
    struct table_scan *ts = malloc(sizeof(struct table_scan));
    memset(ts, 0, sizeof(struct table_scan));
    
    assert(ts != NULL);

    ts->type = TABLE_SCAN;
    ts->ti = ti;
    ts->next = &table_scan_next;
    ts->get_value = &table_scan_get_value;
    ts->reset = &table_scan_reset;
    
    open_heap_table(&ts->ht, ts->ti, ctx->tsx, first_am);
    open_heap_iterator(&ts->ht, &ts->i);

    return (struct scan *)ts;  
}

void 
free_scan(struct scan *s)
{   
    struct project_scan *project = NULL;
    struct select_scan  *select  = NULL;
    struct product_scan *product = NULL;
    struct table_scan   *table   = NULL;

    if (s == NULL)
    {
        return;
    }

    switch (s->type)
    {
        case PROJECT_SCAN: 
            project = (struct project_scan *)s;         
            free_scan(project->scan);
            free(project);
            break;
        case SELECT_SCAN:
            select = (struct select_scan *)s;
            free_scan(select->scan);
            free(select);
            break;
        case PRODUCT_SCAN:
            product = (struct product_scan *)s;
            free_scan(product->l);
            free_scan(product->r);
            free(product);
            break;
        case TABLE_SCAN:
            table = (struct table_scan *)s;
            free(table);
            break;
        default:
            break;
    }
}
