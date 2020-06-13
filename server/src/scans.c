
#include <common.h>
#include <defaults.h>
#include <buffers.h>
#include <heap_table.h>
#include <query_context.h>
#include <scans.h>
#include <storage.h>

struct resolved_value
{
    struct value v;
    char *name;
};

static bool 
select_scan_next(struct scan *scan);

static void 
select_scan_get_value(struct scan *scan, char *column_name, struct value *value);

static bool 
table_scan_next(struct scan *scan);

static void
table_scan_get_value(struct scan *scan, char *name, struct value *v);

void
resolve(vector_type(struct resolved_value) values, char *name, struct value *v)
{
    for (size_t i = 0; i < vector_size(values); i++)
    {
        if (strncmp(name, values[i].name, MAX_COLUMN_NAME) == 0)
        {
            *v = values[i].v;
        }
    }

}

struct value
eval_expr(struct expr *expr, vector_type(struct resolved_value) resolved)
{
    struct term_expr *term;
    struct value v;
    switch(expr->type)
    {
        case EXPR_IDENIFIER:
            term = (struct term_expr *)expr;
            resolve(resolved, term->value.string, &v);

            return v;
        default:
            exit(EXIT_FAILURE);
    }
}

struct scan_field
eval(struct expr *expr)
{
    struct term_expr *term;
    struct infix_expr *infix = NULL;
    
    struct scan_field r;
    struct scan_field lr, rr;

    r.type = EXPR_INTEGER;
    r.value.number = 0;
    r.error = false;
    switch(expr->type)
    {
        case EXPR_INTEGER:
            term = (struct term_expr *)expr;
            r.value.number = term->value.number;
            r.type = EXPR_INTEGER;
            return r;
        case EXPR_IDENIFIER:
        case EXPR_STRING:
            term = (struct term_expr *)expr;
            r.value.s = term->value.string;
            r.type = EXPR_IDENIFIER;
            return r;
        case EXPR_ADD:
        case EXPR_SUB:
        case EXPR_MUL:
        case EXPR_DIV:
        case EXPR_COMPARISON:
            infix = (struct infix_expr *)expr;
            lr = eval(infix->l);
            rr = eval(infix->r);
           
            // Type saftey!  Also, only ints for now
            if (lr.type != rr.type && lr.type == EXPR_INTEGER)
            {
                r.error = true;
                goto end;
            }

            switch (infix->type)
            {
                case EXPR_ADD:
                    r.value.number = lr.value.number + rr.value.number;
                    break;
                case EXPR_SUB:
                    r.value.number = lr.value.number - rr.value.number;
                    break;
                case EXPR_MUL:
                    r.value.number = lr.value.number * rr.value.number;
                    break;
                case EXPR_DIV:
                    r.value.number = lr.value.number / rr.value.number;
                    break;
                default:
                    break;
            }

            break;
    }
end:
    return r;
}

static bool 
scan_project_next(struct scan *scan)
{
    struct scan_project *sp = (struct scan_project *)scan;
    vector_type(struct scan_field) scan_fields = NULL;

    if (sp->has_next == false)
    {
        return false;
    }

    if (sp->scan == NULL)
    {
        //  No data on the project
        //  Evaluate expressions and return;
        for (size_t i = 0; i < vector_size(sp->expr_list); i++)
        {
            struct expr_ctx *expr_ctx = sp->expr_list[i];
            struct scan_field r = eval(expr_ctx->expr);

            vector_push(scan_fields, r);
        }
    }
    sp->has_next = false;
    sp->scan_fields = scan_fields;
    return vector_size(scan_fields) > 0 ? true : false;
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

static void 
free_scan_project(struct scan *s)
{
    // Note: do not free sp->expr_list
    //       it "belongs" to struct select

    struct scan_project *sp = (struct scan_project *)s;

    vector_free(sp->scan_fields);
    
    sp->scan_fields = NULL;
    sp->expr_list = NULL;
}

bool 
project_scan_next(struct scan *scan)
{
    struct project_scan *ps = (struct project_scan *)scan;

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

    switch (s->type)
    {
        case PROJECT_SCAN: 
            project = (struct project_scan *)s;
            free_scan_project(s);
            free_scan(s->scan);
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

    return ss->scan->next(ss->scan);
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
