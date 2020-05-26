#ifndef __PLANS_H__
#define __PLANS_H__

#include <scans.h>
#include <sql.h>
#include <vector.h>

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
    struct scan * (*open)(struct plan *project);

    vector_type(struct expr *) expr_list;
};

struct plan * create_no_data_select_plan(struct select *select);

#endif
