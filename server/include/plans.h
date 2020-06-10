#ifndef __PLANS_H__
#define __PLANS_H__

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

struct plan
{
    enum plan_type type;
    struct scan *(*open)(struct plan *plan);
    vector_type(struct column) column_list;
};

struct project_plan
{
    enum plan_type type;
    struct scan * (*open)(struct plan *project);
    vector_type(struct column) column_list;
    vector_type(struct expr *) expr_list;

    struct plan *p;
};

struct select_plan
{
    enum plan_type type;
    struct scan * (*open)(struct plan *project);
    vector_type(struct column) column_list;
    vector_type(struct expr *) expr_list;
    
    struct plan *p;
    struct expr *where_predicate;
};

struct product_plan
{
    enum plan_type type;
    struct scan * (*open)(struct plan *project);
    vector_type(struct column) column_list;
    vector_type(struct expr *) expr_list;

    struct plan *l;
    struct plan *r;
};

struct table_plan
{
    enum plan_type type;
    struct scan *(*open)(struct plan *plan);
    vector_type(struct column) column_list;
    struct table_info *ti;
    PNUM first_am;
};


struct plan * new_no_data_select_plan(struct select *select);
struct plan * new_select_stmt_plan(struct select *select, struct syscat *cat, struct transaction *tsx);

void free_plan(struct plan *plan);

#endif
