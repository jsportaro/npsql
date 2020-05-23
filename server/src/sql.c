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

    select->expr_list = NULL;
    select->table_refs = NULL;
    select->type = STMT_SELECT;
    select->expr_list = expr_list;

    return (struct sql_stmt *)select;
}

struct sql_stmt *
new_select_data(
    vector_type(struct expr *) expr_list, 
    vector_type(struct table_ref *) table_refs, 
    struct expr *where)
{
     struct select *select = malloc(sizeof(struct select));

    select->expr_list = NULL;
    select->table_refs = NULL;
    select->where = NULL;
    select->type = STMT_SELECT;
    select->expr_list = expr_list;
    select->table_refs = table_refs;
    select->where = where;
    
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

struct table_ref * 
new_table_ref(const char *name)
{
    struct table_ref * table_ref = malloc(sizeof(struct table_ref));

    table_ref->table_name = malloc(strlen(name) + 1);
    strcpy(table_ref->table_name, name);

    return table_ref;
}

vector_type(struct table_ref *) 
new_table_list(struct table_ref * table_ref)
{
    vector_type(struct table_ref *) table_ref_list = NULL;

    vector_push(table_ref_list, table_ref);

    return table_ref_list;
}

struct expr * 
new_term_expr(enum expr_type type, const void *v)
{
    struct term_expr *expr = (struct term_expr *) malloc(sizeof(struct term_expr));
    
    expr->type = type;
    expr->value.string = NULL;

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

struct expr * 
new_infix_expr(enum expr_type type, struct expr *l, struct expr *r)
{
    struct infix_expr *expr = malloc(sizeof(struct infix_expr));

    expr->l = NULL;
    expr->r = NULL;

    expr->type = type;
    expr->l = l;
    expr->r = r;

    return (struct expr *)expr;
}


void free_select(struct select *select);
void free_table_ref(struct table_ref *table_ref);
void free_expr(struct expr *expr);

void 
free_stmts(struct parsed_sql * parsed)
{
    for (size_t i = 0; i < vector_size(parsed->stmts); i++)
    {
        switch(parsed->stmts[i]->type)
        {
            case STMT_SELECT:
                free_select((struct select *)parsed->stmts[i]);
                break;
        }

        free(parsed->stmts[i]);
    }

    vector_free(parsed->stmts);
    free(parsed);
}

void 
free_select(struct select * select)
{
    for (size_t i = 0; i < vector_size(select->expr_list); i++)
    {
        free_expr(select->expr_list[i]);
    }

    for (size_t i = 0; i < vector_size(select->table_refs); i++)
    {
        free_table_ref(select->table_refs[i]);
    }

    vector_free(select->expr_list);
    vector_free(select->table_refs);
}

void 
free_table_ref(struct table_ref *table_ref)
{
    free(table_ref->table_name);
    free(table_ref);
}

void 
free_expr(struct expr *expr)
{
    struct infix_expr *infix = NULL;
    struct term_expr *term = NULL;

    switch(expr->type)
    {
        case EXPR_INTEGER:
            free(expr);
            break;
        case EXPR_IDENIFIER:
        case EXPR_STRING:
            term = (struct term_expr *)expr;
            free(term->value.string);
            free(term);
            break;
        case EXPR_ADD:
        case EXPR_SUB:
        case EXPR_MUL:
        case EXPR_DIV:
            infix = (struct infix_expr *)expr;
            free_expr(infix->l);
            free_expr(infix->r);
            free(expr);
            break;
    }
}
