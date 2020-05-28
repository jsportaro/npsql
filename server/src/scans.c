
#include <common.h>
#include <buffer.h>
#include <scans.h>

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
            struct expr *expr = sp->expr_list[i];
            struct scan_field r = eval(expr);

            vector_push(scan_fields, r);
        }
    }
    sp->has_next = false;
    sp->scan_fields = scan_fields;
    return vector_size(scan_fields) > 0 ? true : false;
}

struct scan * 
create_scan_project(vector_type(struct expr *) expr_list, struct scan *scan)
{
    struct scan_project *sp = malloc(sizeof(struct scan_project));

    sp->type = SCAN_PROJECT;
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

void 
free_scan(struct scan *s)
{   
    if (s == NULL)
    {
        return;
    }

    switch (s->type)
    {
        case SCAN_PROJECT: 
            free_scan_project(s);
            free_scan(s->scan);
            free(s);
            break;
        default:
            break;
    }
}
