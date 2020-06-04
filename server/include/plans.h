#ifndef __PLANS_H__
#define __PLANS_H__

#include <scans.h>
#include <sql.h>
#include <storage.h>
#include <vector.h>

enum plan_type
{
    PLAN_PROJECT,
    CREATE_TABLE
};

struct plan
{
    enum plan_type type;
    struct scan *(*open)(struct plan *plan);
};

struct plan_query
{
    enum plan_type type;
    struct scan *(*open)(struct plan *plan);
    vector_type(struct column) column_list;
};

struct plan_project
{
    enum plan_type type;
    struct scan * (*open)(struct plan *project);
    vector_type(struct column) column_list;
    vector_type(struct expr *) expr_list;
};

struct plan * new_no_data_select_plan(struct select *select);
struct plan * new_create_table_plan(struct create_table *create_table);
void free_plan(struct plan *plan);

#endif
