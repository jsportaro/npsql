#ifndef __PLANNER_H__
#define __PLANNER_H__

#include <executor.h>
#include <parser.h>

enum plan_type
{
    PLAN_PROJECT
};

struct plan
{
    enum plan_type type;

    struct scan *(*open)(struct plan *plan);
};

struct project
{
    enum plan_type type;

    vector_type(struct expr *) expr_list;
};

struct plan * create_plan(struct sql_stmt *sql);

#endif
