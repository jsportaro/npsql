#ifndef __SQL_H__
#define __SQL_H__

#include <defaults.h>
#include <list.h>
#include <types.h>
#include <vector.h>

#include <stdbool.h>
#include <stdint.h>

#define TABLE_NAME_SIZE (MAX_TABLE_NAME + 1)
#define COLUMN_NAME_SIZE (MAX_COLUMN_NAME + 1)

enum stmt_type
{
    STMT_SELECT,
    STMT_CREATE_TABLE,
    STMT_INSERT_INTO
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
    EXPR_EQU,
    EXPR_AND,
    EXPR_OR,
};

struct expr_ctx
{
    struct expr *expr;
    char *col_name;
    enum npsql_type type;
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

//  [qualifier].[name]
//  table_name.field for instance
struct identifier
{
    const char *qualifier;
    const char *name;
};

struct term_expr
{
    enum expr_type type;

    union 
    {
        int number;
        char *string;
        struct identifier *identifier;
    } value;
};

struct table_ref
{
    const char *table_name;
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

struct select
{
    enum stmt_type type;

    vector_type(struct expr_ctx *) expr_ctx_list;
    vector_type(struct table_ref *) table_refs;
    struct expr *where;

    vector_type(struct identifier *) unresolved;
};

struct create_table
{
    enum stmt_type type;
    const char *table_name;
    vector_type(struct column_def *) column_defs;
};

struct insert
{
    enum stmt_type type;
    const char *name;
    vector_type(char *) columns;
    vector_type(struct expr *) values;
};

typedef struct SqlStatement 
{
    ListHead list;
    enum 
    {
        STATEMENT_SELECT,
        STATEMENT_SELECT_STAR,
        STATEMENT_CREATE_TABLE,
        STATEMENT_INSERT
    } type;

    union {
        struct
        { 
            vector_type(struct expr_ctx *) expr_ctx_list;
            vector_type(struct table_ref *) table_refs;
            struct expr *where;
            vector_type(struct identifier *) unresolved;
        } select;

        struct
        { 
            const char *table_name;
            vector_type(struct column_def *) column_defs;
        } create_table;

        struct
        { 
            const char *name;
            vector_type(char *) columns;
            vector_type(struct expr *) values;
        } insert;
    };
} SqlStatement;

struct parsed_sql
{
    vector_type(char) error_msg;
    bool error;
    
    ListHead statements;

    vector_type(struct sql_stmt *) stmts;
    vector_type(struct identifier *) unresolved;
};

SqlStatement *NewSelectStatement(
    vector_type(struct expr_ctx *) expr_ctx_list, 
    vector_type(struct table_ref *) table_refs, 
    struct expr *where,
    vector_type(struct identifier *) unresolved);

vector_type(struct expr *) new_expr_list(struct expr *expr);
vector_type(struct expr *) append_expr_list(vector_type(struct expr *) expr_list, struct expr *expr);

vector_type(struct expr_ctx *) new_expr_ctx_list(struct expr *expr);
vector_type(struct expr_ctx *) append_expr_ctx_list(vector_type(struct expr_ctx *) expr_ctx_list, struct expr *expr);

vector_type(struct table_ref *) new_table_list(struct table_ref * table_ref);
vector_type(struct table_ref *) append_table_list(vector_type(struct table_ref *) table_refs, struct table_ref * table_ref);
struct table_ref * new_table_ref(const char *name);

struct term_expr * new_term_expr(enum expr_type type, const void *v);
struct term_expr * new_identifier(const char *qualifier, const char *name);
struct expr * new_infix_expr(enum expr_type type, struct expr *l, struct expr *r);

SqlStatement *NewCreateTableStatement(const char *name, vector_type(struct column_def *) column_defs);
vector_type(struct column_def *) new_column_def_list(struct column_def *column_def);
vector_type(struct column_def *) append_column_def_list(vector_type(struct column_def *) column_def_list, struct column_def *column_def);
struct column_def * create_column_def(const char *name,  struct type_def *type);
struct type_def * create_type_def(enum npsql_type type, uint16_t size);

SqlStatement *NewInsertStatement(const char *name, vector_type(char *) columns, vector_type(struct expr *) values);
vector_type(char *) new_column_list(const char *column);
vector_type(char *) append_column_list(vector_type(char *) column_list, const char *column);

void free_stmts(struct parsed_sql * parsed);

#endif
