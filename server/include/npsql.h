#ifndef __NPSQL_H__
#define __NPSQL_H__

#include <common.h>
#include <vector.h>
#include <parser.h>
#include <query_context.h>
#include <sql.h>
#include <syscat.h>
#include <transaction.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DB_OK 1


struct query_engine
{
    struct transaction_context ctx;
    struct syscat              cat;

};

struct query_results
{
    uint32_t                         sets_to_return;
    int                              rows_to_return;
    vector_type(uint8_t)             current;
    struct parsed_sql               *parsed_sql;
    uint32_t                         next_stmt;
    vector_type(struct plan_column)  columns;
    struct plan                     *current_plan;
    struct scan                     *current_scan;
    struct query_engine             *engine;
    struct transaction              *tsx;
    struct query_ctx                 ctx;
};


int query_engine_init(const char *data_file, const char *log_file, struct query_engine *query_engine);
void free_query_engine(struct query_engine *query_engine);
struct query_results * submit_query(struct query_engine *query_engine, char *query, size_t length);
vector_type(struct expr_ctx *) get_sql_select(struct query_results *r);
bool get_next_set(struct query_results *results);
bool has_rows(struct query_results *r);
bool next_set_record(struct query_results *results);
void free_results(struct query_results *results);
vector_type(uint8_t) get_row_bytes(struct query_results *results);

#endif
