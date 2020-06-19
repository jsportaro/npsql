#include <common.h>
#include <expr_eval.h>
#include <value.h>

bool
do_op(enum expr_type t, struct value *l, struct value *r,struct value *v)
{
    
    v->type = TYPE_INT;
    v->size = TYPE_INT_SIZE;
    
    switch (t)
    {
        case EXPR_ADD:
            v->as.number = l->as.number + r->as.number;
            return true;
        case EXPR_SUB:
            v->as.number = l->as.number - r->as.number;
            return true;
        case EXPR_MUL:
            v->as.number = l->as.number * r->as.number;
            return true;
        case EXPR_DIV:
            v->as.number = l->as.number / r->as.number;
            return true;
        case EXPR_EQU:
            if (l->type == TYPE_INT)
            {
                v->type = TYPE_BOOL;
                v->as.boolean = l->as.number == r->as.number;

                return true;
            }
            break;
        case EXPR_OR:
            if (l->type == TYPE_BOOL && r->type == TYPE_BOOL)
            {
                v->type = TYPE_BOOL;
                v->as.boolean = l->as.boolean || r->as.boolean;

                return true;
            }
            break;
        case EXPR_AND:
            if (l->type == TYPE_BOOL && r->type == TYPE_BOOL)
            {
                v->type = TYPE_BOOL;
                v->as.boolean = l->as.boolean && r->as.boolean;

                return true;
            }
            break;
        default:
            return false;
    }

    return false;
}

struct value 
eval(struct expr *expr, struct scan *scan)
{
    struct value v = { 0 };
    struct value r = { 0 };
    struct value l = { 0 };

    struct term_expr *term;
    struct infix_expr *infix = NULL;

    switch(expr->type)
    {
        case EXPR_IDENIFIER:
            term = (struct term_expr *)expr;
            scan->get_value(scan, term->value.identifier, &v);
            break;
        case EXPR_INTEGER:
            term = (struct term_expr *)expr;
            v.type = TYPE_INT;
            v.size = TYPE_INT_SIZE;
            v.as.number = term->value.number;
            break;
        case EXPR_ADD:
        case EXPR_SUB:
        case EXPR_MUL:
        case EXPR_DIV:
        case EXPR_EQU:
        case EXPR_AND:
        case EXPR_OR:
            infix = (struct infix_expr *)expr;
            l = eval(infix->l, scan);
            r = eval(infix->r, scan);

            if (do_op(infix->type, &l, &r, &v) != true)
            {
                v.type = TYPE_UNKNOWN;
            }

            // Cleanup
            reset(&l);
            reset(&r);
            break;
        default:
            v.type = TYPE_UNKNOWN;
            break;
    }

    return v;
}
