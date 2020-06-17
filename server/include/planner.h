#ifndef __PLANNER_H__
#define __PLANNER_H__

#include <defaults.h>
#include <executor.h>
#include <parser.h>
#include <plans.h>
#include <query_context.h> 

enum planner_status
{
    PLANNER_SUCCESS,
    PLANNER_ERROR
};

struct planner_result
{
    struct plan *plan;
    vector_type(struct plan_column) columns;
    char *message;
    enum planner_status status;
};

struct planner_result * create_plan(struct sql_stmt *sql, struct query_ctx *ctx);


#endif
