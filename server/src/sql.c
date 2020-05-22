#include <sql.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct sql_stmt *new_select
(vector_type(struct expr *) expr_list)
{
    struct select *select = malloc(sizeof(struct select));

    select->type = STMT_SELECT;
    select->expr_list = expr_list;

    return (struct sql_stmt *)select;
}

vector_type(struct expr *)
new_expr_list(struct expr *expr)
{
    vector_type(struct expr *) expr_list = NULL;

    vector_push(expr_list, expr);

    return expr_list;
}

vector_type(struct expr *)
append_expr_list(vector_type(struct expr *) expr_list, struct expr *expr)
{
    vector_push(expr_list, expr);

    return expr_list;
}

struct expr * 
new_term_expr(enum expr_type type, const void *v)
{
    struct term_expr *expr = (struct term_expr *) malloc(sizeof(struct term_expr));
    expr->type = type;
    switch (type) {
        case EXPR_STRING:
        case EXPR_IDENIFIER:
            expr->value.string = (char *) malloc(strlen((char *) v)+1);
            strcpy(expr->value.string, (char *) v);
            break;
        case EXPR_INTEGER:
            expr->value.number = *((long *) v);
            break;
        default:
            break;
    }
    return (struct expr * )expr;
}

struct expr * new_infix_expr(enum expr_type type, struct expr *l, struct expr *r)
{
    struct infix_expr *expr = malloc(sizeof(struct infix_expr));

    expr->type = type;
    expr->l = l;
    expr->r = r;

    return (struct expr *)expr;
}
