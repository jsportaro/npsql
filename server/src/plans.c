#include <common.h>
#include <plans.h>

#include <stdlib.h>
#include <string.h>

struct scan * 
open_no_data_select_scan(struct plan *plan)
{
    struct plan_project *p = (struct plan_project *)plan;
    
    return create_scan_project(p->expr_list, NULL);
}

struct column
bind_column(struct expr *expr)
{
    struct term_expr *term;
    struct infix_expr *infix = NULL;
    UNUSED(term);
    struct column r;
    struct column lr, rr;

    r.name   = NULL;
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

    for (size_t i = 0; i < vector_size(select->expr_list); i++)
    {
        struct expr *expr = select->expr_list[i];
        struct column c = bind_column(expr);
        vector_push(pc, c);
    }

    return pc;
}

struct plan * 
new_no_data_select_plan(struct select *select)
{
    struct plan_project *p = malloc(sizeof(struct plan_project));

    p->type = PLAN_PROJECT;
    p->expr_list = select->expr_list;
    p->column_list = bind_columns(select);
    p->open = &open_no_data_select_scan;

    return (struct plan *)p;
}

static void
free_plan_project(struct plan *p)
{
    // Note: do not free pp->expr_list
    //       it "belongs" to struct select

    struct plan_project *pp = (struct plan_project *)p;

    vector_free(pp->column_list);
}

void 
free_plan(struct plan *p)
{
    if (p == NULL)
    {
        return;
    }

    switch (p->type)
    {
        case PLAN_PROJECT:
            free_plan_project(p);
          
            free(p);
            break;
        default:
            
            break;
    }
}
