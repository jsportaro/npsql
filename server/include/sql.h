#ifndef __SQL_H__
#define __SQL_H__

#include <defaults.h>
#include <types.h>
#include <vector.h>

#include <stdbool.h>
#include <stdint.h>

#define TABLE_NAME_SIZE (MAX_TABLE_NAME + 1)
#define COLUMN_NAME_SIZE (MAX_COLUMN_NAME + 1)
enum stmt_type
{
    STMT_SELECT,
    STMT_CREATE_TABLE
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
    EXPR_DIV,
    EXPR_COMPARISON
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

struct table_ref
{
    const char *table_name;
};

struct select
{
    enum stmt_type type;

    vector_type(struct expr *) expr_list;
    vector_type(struct table_ref *) table_refs;

    struct expr *where;
};

struct create_table
{
    enum stmt_type type;
    const char *table_name;
    vector_type(struct column_def *) column_defs;
};

struct type_def
{  
    enum npsql_type type;
    uint16_t size;
};

struct column_def
{
    const char *name;
    struct type_def *type;
};

struct sql_stmts
{
    vector_type(struct sql_stmts *) stmts;
};

struct parsed_sql
{
    vector_type(char) error_msg;
    bool error;
    
    vector_type(struct sql_stmt *) stmts;
};

void append_stmt(vector_type(struct sql_stmt *) stmt_list, struct sql_stmt * stmt);

struct sql_stmt * new_select_data(
    vector_type(struct expr *) expr_list, 
    vector_type(struct table_ref *) table_refs, 
    struct expr *where);

struct sql_stmt * new_select(vector_type(struct expr *) expr_list);

vector_type(struct expr *) new_expr_list(struct expr *expr);
vector_type(struct expr *) append_expr_list(vector_type(struct expr *) expr_list, struct expr *expr);

vector_type(struct table_ref *) new_table_list(struct table_ref * table_ref);
struct table_ref * new_table_ref(const char *name);

struct expr * new_term_expr(enum expr_type type, const void *v);
struct expr * new_infix_expr(enum expr_type type, struct expr *l, struct expr *r);

struct sql_stmt * new_create_table(const char *name, vector_type(struct column_def *) column_defs);
vector_type(struct column_def *) new_column_def_list(struct column_def *column_def);
vector_type(struct column_def *) append_column_def_list(vector_type(struct column_def *) column_def_list, struct column_def *column_def);
struct column_def * create_column_def(const char *name,  struct type_def *type);
struct type_def * create_type_def(enum npsql_type type, uint16_t size);

void free_stmts(struct parsed_sql * parsed);

#endif
