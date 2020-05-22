#ifndef __SQL_H__
#define __SQL_H__

#include <vector.h>

#include <stdint.h>

enum stmt_type
{
    STMT_SELECT
};

struct sql_stmt
{
    enum stmt_type type;
};

enum expr_type
{
    EXPR_INTEGER,
    EXPR_STRING,
    EXPR_IDENIFIER,
    EXPR_ADD,
    EXPR_SUB,
    EXPR_MUL,
    EXPR_DIV
};

struct expr
{
    enum expr_type type;
};

struct infix_expr
{
    enum expr_type type;

    struct expr *l;
    struct expr *r;
};

struct term_expr
{
    enum expr_type type;

    union 
    {
        int number;
        char *string;
    } value;
};

struct select
{
    enum stmt_type type;

    vector_type(struct expr *) expr_list;
};

struct sql_stmts
{
    vector_type(struct sql_stmts *) stmts;
};

struct parsed_sql
{
    struct sql_stmt *sql;
};

struct sql_stmt *new_select(vector_type(struct expr *) expr_list);
vector_type(struct expr *) new_expr_list(struct expr *expr);
vector_type(struct expr *) append_expr_list(vector_type(struct expr *) expr_list, struct expr *expr);

struct expr * new_term_expr(enum expr_type type, const void *v);
struct expr * new_infix_expr(enum expr_type type, struct expr *l, struct expr *r);


#endif
