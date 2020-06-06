#include <common.h>
#include <create_table.h>
#include <npsql.h>
#include <networking.h>
#include <parser.h>
#include <plans.h>
#include <planner.h>
#include <sql.h>
#include <transaction.h>

#include <stdio.h>
#include <string.h>

int query_engine_init(const char *data_file, const char *log_file , struct query_engine *query_engine)
{
    initialize_transaction_context(&query_engine->ctx, data_file, log_file);

    return DB_OK;
}

struct query_results * submit_query(struct query_engine *query_engine, char *query, size_t length)
{
    struct query_results *results = malloc(sizeof(struct query_results));

    results->parsed_sql = parse_sql(query, length);
    results->next_stmt = 0;
    results->sets_to_return = vector_size(results->parsed_sql->stmts);

    results->rows_to_return = 0;
    results->current = NULL;
    results->current_plan = NULL;
    results->current_scan = NULL;
    results->engine = query_engine;     

    return results;
}

bool has_rows(struct query_results *r)
{
    return r->current_scan->has_rows;
}

bool get_next_set(struct query_results *r)
{
    if (r->sets_to_return == r->next_stmt)
    {
        return false;
    }
    struct sql_stmt *s = r->parsed_sql->stmts[r->next_stmt];

    free_plan(r->current_plan);
    free_scan(r->current_scan);
    
    if (s->type == STMT_SELECT)
    {
        r->current_plan =  create_plan(s);
        r->current_scan = r->current_plan->open(r->current_plan);
        r->next_stmt++;
    }
    else if (s->type == STMT_CREATE_TABLE)
    {

    }

    return true;
}

void free_results(struct query_results *r)
{
    // for (size_t i = 0; i < vector_size(r->current_plan->column_list); i++)
    // {
    //     struct column col = r->current_plan->column_list[i];
    //     vector_free(col.name);
    // }

    // vector_free(r->current_columns);
    free_stmts(r->parsed_sql);
    free_plan(r->current_plan);
    free_scan(r->current_scan);
    free(r);
}

bool next_record(struct query_results *r)
{
    return r->current_scan->next(r->current_scan);
}
