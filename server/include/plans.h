#ifndef __PLANS_H__
#define __PLANS_H__

#include <query_context.h>
#include <scans.h>
#include <sql.h>
#include <storage.h>
#include <syscat.h>
#include <transaction.h>
#include <vector.h>

enum plan_type
{
    PROJECT_PLAN,
    PRODUCT_PLAN,
    SELECT_PLAN,
    TABLE_PLAN,
    CREATE_TABLE
};

struct plan_column
{
    size_t size;
    char *name;
    enum npsql_type type;
    struct expr *expr;
};

struct plan
{
    enum plan_type type;
    struct scan *(*open)(struct plan *plan);
    bool (*get_column)(struct plan *plan, char *name, struct plan_column *column);
    vector_type(struct plan_column *) (*get_columns)(struct plan *plan);
};

struct project_plan
{
    enum plan_type type;
    struct scan * (*open)(struct plan *project);
    bool (*get_column)(struct plan *plan, char *name, struct plan_column *column);
    vector_type(struct plan_column *) (*get_columns)(struct plan *plan);

    struct plan *p;
};

struct select_plan
{
    enum plan_type type;
    struct scan * (*open)(struct plan *project);
    bool (*get_column)(struct plan *plan, char *name, struct plan_column *column);
    vector_type(struct plan_column *) (*get_columns)(struct plan *plan);

    struct plan *p;
    struct expr *where_clause;
};

struct product_plan
{
    enum plan_type type;
    struct scan * (*open)(struct plan *project);
    bool (*get_column)(struct plan *plan, char *name, struct plan_column *column);
    vector_type(struct plan_column *) (*get_columns)(struct plan *plan);

    struct plan *l;
    struct plan *r;
    vector_type(struct plan_column *) ppc;
};

struct table_plan
{
    enum plan_type type;
    struct scan *(*open)(struct plan *plan);
    bool (*get_column)(struct plan *plan, char *name, struct plan_column *column);
    vector_type(struct plan_column *) (*get_columns)(struct plan *plan);

    struct query_ctx *ctx;
    struct table_info ti;
    vector_type(struct plan_column *) columns;
    PNUM first_am;
};


struct plan * new_no_data_select_plan(struct select *select);
struct plan * new_select_stmt_plan(struct select *select, struct query_ctx *ctx);

void free_plan(struct plan *plan);

#endif
