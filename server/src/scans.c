#include <buffers.h>
#include <common.h>
#include <defaults.h>
#include <expr_eval.h>
#include <heap_table.h>
#include <query_context.h>
#include <scans.h>
#include <storage.h>

static bool 
select_scan_next(struct scan *scan);

static void 
select_scan_get_value(struct scan *scan, char *column_name, struct value *value);

static bool 
table_scan_next(struct scan *scan);

static void
table_scan_get_value(struct scan *scan, char *name, struct value *v);


static bool 
scan_project_next(struct scan *scan)
{
    struct scan_project *sp = (struct scan_project *)scan;
    
    return sp->scan->next(sp->scan);
}

struct scan * 
new_scan_project(vector_type(struct expr_ctx *) expr_list, struct scan *scan)
{
    struct scan_project *sp = malloc(sizeof(struct scan_project));

    sp->type = PROJECT_SCAN;
    sp->expr_list = expr_list;
    sp->next = &scan_project_next;
    sp->scan = scan;
    sp->has_next = true;
    sp->has_rows = true;

    return (struct scan *)sp;
}

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
project_scan_get_value(struct scan *scan, char *column_name, struct value *value)
{
    struct project_scan *ps = (struct project_scan *)scan;
    
    ps->scan->get_value(ps->scan, column_name, value);
    
    return;
}

struct scan * 
new_project_scan(struct scan *inner)
{
    struct project_scan *ps = malloc(sizeof(struct project_scan));
    assert(ps != NULL);

    ps->type = PROJECT_SCAN;
    ps->scan = inner;
    ps->next = &project_scan_next;
    ps->get_value = &project_scan_get_value;
    ps->has_next = true;

    return (struct scan *)ps;
}

struct scan * 
new_select_scan(struct scan *inner, struct expr *where_clause)
{
    struct select_scan *ss = malloc(sizeof(struct select_scan));
    assert(ss != NULL);

    ss->type = SELECT_SCAN;
    ss->scan = inner;
    ss->where_clause = where_clause;
    ss->next = &select_scan_next;
    ss->get_value = &select_scan_get_value;
    
    return (struct scan *)ss;
}

struct scan * 
new_product_scan(struct scan *l, struct scan *r)
{
    struct product_scan *ps = malloc(sizeof(struct product_scan));
    assert(ps != NULL);

    ps->type = PRODUCT_SCAN;
    ps->l = l;
    ps->r = r;

    return (struct scan *)ps;
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

    free_scan(s->scan);

    switch (s->type)
    {
        case PROJECT_SCAN: 
            project = (struct project_scan *)s;
            free(project);
            break;
        case SELECT_SCAN:
            select = (struct select_scan *)s;
            free(select);
            break;
        case PRODUCT_SCAN:
            product = (struct product_scan *)s;
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
select_scan_get_value(struct scan *scan, char *column_name, struct value *value)
{
    struct select_scan *ss = (struct select_scan *)scan;

    ss->scan->get_value(ss->scan, column_name, value);
}

static bool 
table_scan_next(struct scan *scan)
{
    struct table_scan *ts = (struct table_scan *)scan;

    return next_record(&ts->i);
}

static void
table_scan_get_value(struct scan *scan, char *name, struct value *v)
{
    struct table_scan *ts = (struct table_scan *)scan;
    int i = 0;

    for (; i < ts->ti->column_count; i++)
    {
        if (strncmp(name, ts->ti->columns[i].name, MAX_COLUMN_NAME) == 0)
        {
            break;
        }
    }

    get_value(&ts->ht, ts->i.current_record, i, v);
}
