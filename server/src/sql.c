#include <common.h>
#include <sql.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void 
append_stmt(vector_type(struct sql_stmt *) stmt_list, struct sql_stmt * stmt)
{
    vector_push(stmt_list, stmt);
}

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



void traverse_select(struct select * select, void (*func)(void *object));
void traverse_expr(struct expr *expr, void (*func)(void *object));

static void do_nothing(void *object)
{
    UNUSED(object);
}

void traverse_stmts(struct parsed_sql * parsed, void (*func)(void *object))
{
    if (func == NULL)
    {
        func = &do_nothing;
    }

    for (size_t i = 0; i < vector_size(parsed->stmts); i++)
    {
        switch(parsed->stmts[i]->type)
        {
            case STMT_SELECT:
                traverse_select((struct select *)parsed->stmts[i], func);
                break;
        }
    }
}

static void delete(void *object)
{
    free(object);
}

void delete_stmts(struct parsed_sql * parsed)
{
    traverse_stmts(parsed, &delete);

    free(parsed);
}

void traverse_select(struct select * select, void (*func)(void *object))
{
    fprintf(stdout, "I have %d projections\n", (int)vector_size(select->expr_list));

    for (size_t i = 0; i < vector_size(select->expr_list); i++)
    {
        traverse_expr(select->expr_list[i], func);
        
    }
}

void traverse_expr(struct expr *expr, void (*func)(void *object))
{
    struct infix_expr *infix = NULL;

    switch(expr->type)
    {
        case EXPR_INTEGER:
            func(expr);
            break;
        case EXPR_STRING:
            func(expr);
            break;
        case EXPR_IDENIFIER:
            func(expr);
            break;
        case EXPR_ADD:
        case EXPR_SUB:
        case EXPR_MUL:
        case EXPR_DIV:
            infix = (struct infix_expr *)expr;
            traverse_expr(infix->l, func);
            traverse_expr(infix->r, func);
            func(expr);
            break;
    }

}
