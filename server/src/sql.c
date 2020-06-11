#include <common.h>
#include <sql.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void 
append_stmt(vector_type(struct sql_stmt *) stmt_list, struct sql_stmt * stmt)
{
    vector_push(stmt_list, stmt);
}

struct sql_stmt *new_select
(vector_type(struct expr_ctx) expr_ctx_list)
{
    struct select *select = malloc(sizeof(struct select));

    assert(select != NULL);


    select->expr_ctx_list = NULL;
    select->table_refs = NULL;
    select->where = NULL;
    select->type = STMT_SELECT;

    select->expr_ctx_list = expr_ctx_list;

    return (struct sql_stmt *)select;
}

struct sql_stmt *
new_select_data(
    vector_type(struct expr_ctx) expr_ctx_list, 
    vector_type(struct table_ref *) table_refs, 
    struct expr *where)
{
    struct select *select = malloc(sizeof(struct select));

    assert(select != NULL);

    select->expr_ctx_list = NULL;
    select->table_refs = NULL;
    select->where = NULL;
    select->type = STMT_SELECT;

    select->expr_ctx_list = expr_ctx_list;
    select->table_refs = table_refs;
    select->where = where;

    return (struct sql_stmt *)select;
}

vector_type(struct expr *)
new_expr_list(struct expr *expr, vector_type(char *) unresolved)
{
    vector_type(struct expr *) expr_list = NULL;

    if (expr != NULL)
    {
        vector_push(expr_list, expr);
    }

    for (size_t i = 0; i < vector_size(unresolved); i++)
    {
        printf("%s\n", unresolved[i]);
    }

    return expr_list;
}

vector_type(struct expr *)
append_expr_list(vector_type(struct expr *) expr_list, struct expr *expr, vector_type(char *) unresolved)
{
    vector_push(expr_list, expr);

    for (size_t i = 0; i < vector_size(unresolved); i++)
    {
        printf("%s\n", unresolved[i]);
    }

    return expr_list;
}

vector_type(struct expr_ctx) 
new_expr_ctx_list(struct expr *expr, vector_type(char *) unresolved)
{
    vector_type(struct expr_ctx) expr_ctx_list = NULL;
    struct expr_ctx expr_ctx = { 0 };

    expr_ctx.expr = expr;
    expr_ctx.unresolved = unresolved;
    vector_push(expr_ctx_list, expr_ctx);

    return expr_ctx_list;
}

vector_type(struct expr_ctx) 
append_expr_ctx_list(vector_type(struct expr_ctx) expr_ctx_list, struct expr *expr, vector_type(char *) unresolved)
{
    struct expr_ctx expr_ctx = { 0 };

    expr_ctx.expr = expr;
    expr_ctx.unresolved = unresolved;
    vector_push(expr_ctx_list, expr_ctx);

    return expr_ctx_list;
}

struct table_ref * 
new_table_ref(const char *name)
{
    struct table_ref * table_ref = malloc(sizeof(struct table_ref));

    assert(table_ref != NULL);

    table_ref->table_name = name;

    return table_ref;
}

vector_type(struct table_ref *) 
new_table_list(struct table_ref * table_ref)
{
    vector_type(struct table_ref *) table_ref_list = NULL;

    vector_push(table_ref_list, table_ref);

    return table_ref_list;
}

struct term_expr * 
new_term_expr(enum expr_type type, const void *v)
{
    struct term_expr *expr = (struct term_expr *) malloc(sizeof(struct term_expr));
    
    assert(expr != NULL);

    expr->type = type;
    expr->value.string = NULL;

    switch (type) {
        case EXPR_STRING:
        case EXPR_IDENIFIER:
            expr->value.string = (char *)v;
            break;
        case EXPR_INTEGER:
            expr->value.number = *((long *) v);
            break;
        default:
            break;
    }
    return expr;
}

struct expr * 
new_infix_expr(enum expr_type type, struct expr *l, struct expr *r)
{
    struct infix_expr *expr = malloc(sizeof(struct infix_expr));

    assert(expr != NULL);

    expr->l = NULL;
    expr->r = NULL;

    expr->type = type;
    expr->l = l;
    expr->r = r;

    return (struct expr *)expr;
}

struct sql_stmt * 
new_create_table(const char *name, vector_type(struct column_def *) column_defs)
{
    struct create_table *ct = malloc(sizeof(struct create_table));

    assert(ct != NULL);

    ct->type = STMT_CREATE_TABLE;
    ct->table_name = name;
    ct->column_defs = column_defs;

    return (struct sql_stmt *)ct;
}

vector_type(struct column_def *) 
new_column_def_list(struct column_def *column_def)
{
    vector_type(struct column_def *)  column_def_list = NULL;

    vector_push(column_def_list, column_def);

    return column_def_list;
}

vector_type(struct column_def *) 
append_column_def_list(vector_type(struct column_def *) column_def_list, struct column_def *column_def)
{
    vector_push(column_def_list, column_def);

    return column_def_list;
}

struct column_def * 
create_column_def(const char *name, struct type_def *type)
{
    struct column_def *cd = malloc(sizeof(struct column_def));

    assert(cd != NULL);

    cd->name = name;
    cd->type = type;

    return cd;
}

struct type_def * 
create_type_def(enum npsql_type type, uint16_t size)
{
    struct type_def *td = malloc(sizeof(struct type_def));

    assert(td != NULL);

    td->type = type;
    td->size = size; 

    return td;
}

struct sql_stmt *
new_insert(const char *name, vector_type(char *) columns, vector_type(struct expr *) values)
{
    struct insert *insert = malloc(sizeof(struct insert));

    insert->type = STMT_INSERT_INTO;
    insert->columns = columns;
    insert->values = values;
    insert->name = name;

    return (struct sql_stmt *)insert;
}

vector_type(char *) new_column_list(const char *column)
{
    vector_type(char *) column_list = NULL;

    vector_push(column_list, (char *)column);

    return column_list;
}

vector_type(char *) append_column_list(vector_type(char *) column_list, const char *column)
{
    vector_push(column_list, (char *)column);

    return column_list;
}

void free_select(struct select *select);
void free_create_table(struct create_table *create_table);
void free_insert_into(struct insert *insert);
void free_table_ref(struct table_ref *table_ref);
void free_column_def(struct column_def *column_def);
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
            case STMT_CREATE_TABLE:
                free_create_table((struct create_table *)parsed->stmts[i]);
                break;
            case STMT_INSERT_INTO:
                free_insert_into((struct insert *)parsed->stmts[i]);
        }

        free(parsed->stmts[i]);
    }

    vector_free(parsed->stmts);
    vector_free(parsed->error_msg);

    parsed->stmts = NULL;
    parsed->error_msg = NULL;

    free(parsed);
}

void 
free_select(struct select * select)
{
    for (size_t i = 0; i < vector_size(select->expr_ctx_list); i++)
    {
        free_expr(select->expr_ctx_list[i].expr);
    }

    for (size_t i = 0; i < vector_size(select->table_refs); i++)
    {
        free_table_ref(select->table_refs[i]);
    }

    if (select->where != NULL)
    {
        free_expr(select->where);
    }

    vector_free(select->expr_ctx_list);
    vector_free(select->table_refs);

    select->expr_ctx_list = NULL;
    select->table_refs = NULL;
    select->where = NULL;
}

void
free_create_table(struct create_table *create_table)
{
    for (size_t i = 0; i < vector_size(create_table->column_defs); i++)
    {
        free_column_def(create_table->column_defs[i]);
    }

    vector_free(create_table->column_defs);
    free((char *)create_table->table_name);
}

void free_insert_into(struct insert *insert)
{
    for (size_t i = 0; i < vector_size(insert->columns); i++)
    {
        free(insert->columns[i]);
    }

    for (size_t i = 0; i < vector_size(insert->values); i++)
    {
        free_expr(insert->values[i]);
    }

    free((char *)insert->name);
    vector_free(insert->columns);
    vector_free(insert->values);
}

void 
free_table_ref(struct table_ref *table_ref)
{
    free((char *)table_ref->table_name);
    free(table_ref);
}

void free_column_def(struct column_def *column_def)
{
    free((char *)column_def->name);
    free(column_def->type);
    free(column_def);
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
        case EXPR_COMPARISON:
            infix = (struct infix_expr *)expr;
            free_expr(infix->l);
            free_expr(infix->r);
            free(expr);
            break;
    }
}
