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
};

struct plan
{
    enum plan_type type;
    struct scan *(*open)(struct plan *plan);
    bool (*get_column)(struct plan *plan, char *name, struct plan_column *column);
};

struct project_plan
{
    enum plan_type type;
    struct scan * (*open)(struct plan *project);
    bool (*get_column)(struct plan *plan, char *name, struct plan_column *column);

    struct plan *p;
};

struct select_plan
{
    enum plan_type type;
    struct scan * (*open)(struct plan *project);
    bool (*get_column)(struct plan *plan, char *name, struct plan_column *column);
    
    struct plan  *p;
    struct where *where_clause;
};

struct product_plan
{
    enum plan_type type;
    struct scan * (*open)(struct plan *project);
    bool (*get_column)(struct plan *plan, char *name, struct plan_column *column);

    struct plan *l;
    struct plan *r;
};

struct table_plan
{
    enum plan_type type;
    struct scan *(*open)(struct plan *plan);
    bool (*get_column)(struct plan *plan, char *name, struct plan_column *column);

    struct query_ctx *ctx;
    struct table_info ti;
    PNUM first_am;
};


struct plan * new_no_data_select_plan(struct select *select);
struct plan * new_select_stmt_plan(struct select *select, struct query_ctx *ctx);

void free_plan(struct plan *plan);

#endif
