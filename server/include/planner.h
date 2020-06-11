#ifndef __PLANNER_H__
#define __PLANNER_H__

#include <executor.h>
#include <parser.h>
#include <plans.h>
#include <query_context.h> 

struct plan * create_plan(struct sql_stmt *sql, struct query_ctx *ctx);

#endif
