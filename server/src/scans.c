
#include <common.h>
#include <scans.h>

// void write_result(int ordinal, uint8_t **buffer, size_t *written)
// {

// }



struct eval_res
{
    bool error;
    enum column_types type;
    union {
        int number;
        char *s;
    } value;
};

struct eval_res
eval(struct expr *expr)
{
    struct term_expr *term;
    struct infix_expr *infix = NULL;
    
    struct eval_res r;
    struct eval_res lr, rr;

    r.type = TYPE_INT;
    r.value.number = 0;
    r.error = false;
    switch(expr->type)
    {
        case EXPR_INTEGER:
            term = (struct term_expr *)expr;
            r.value.number = term->value.number;
            r.type = TYPE_INT;
            return r;
        case EXPR_IDENIFIER:
        case EXPR_STRING:
            term = (struct term_expr *)expr;
            r.value.s = term->value.string;
            r.type = TYPE_CHAR;
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
            if (lr.type != rr.type && lr.type == TYPE_INT)
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
    
    if (sp->hasNext == true)
    {
        if (sp->scan == NULL)
        {
            //  No data on the project
            //  Evaluate expressions and return;
             for (size_t i = 0; i < vector_size(sp->expr_list); i++)
            {
                struct expr *expr = sp->expr_list[i];
                struct eval_res r = eval(expr);

                UNUSED(r);
            }
        }
    }

    return sp->hasNext;
}

struct scan * 
create_scan_project(vector_type(struct expr *) expr_list, struct scan *scan)
{
    struct scan_project *sp = malloc(sizeof(struct scan_project));

    sp->type = SCAN_PROJECT;
    sp->expr_list = expr_list;
    sp->next = &scan_project_next;
    sp->scan = scan;
    sp->hasNext = true;
    sp->has_rows = true;

    return (struct scan *)sp;
}

vector_type(struct column *) 
get_scan_columns(struct scan *s)
{
    vector_type(struct column *) c = NULL;

    UNUSED(s);

    return c;
}
