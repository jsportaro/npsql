#include <expr_eval.h>

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