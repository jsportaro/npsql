#include <common.h>
#include <npsql.h>
#include <networking.h>
#include <parser.h>
#include <plans.h>
#include <planner.h>
#include <sql.h>

#include <stdio.h>
#include <string.h>

int query_engine_init(char *data_file, char *log_file , struct query_engine *query_engine)
{
    UNUSED(data_file);
    UNUSED(log_file);
    UNUSED(query_engine);

    return DB_OK;
}

struct query_results * submit_query(struct query_engine *query_engine, char *query, size_t length)
{
    struct query_results *results = malloc(sizeof(struct query_results));

    results->parsed_sql = parse_sql(query, length);
    results->next_stmt = 0;
    results->sets_to_return = vector_size(results->parsed_sql->stmts);

    results->rows_to_return = 104;
    results->current = NULL;
    results->current_plan = NULL;
    results->current_scan = NULL;
    
    UNUSED(query_engine);

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

    free_plan(r->current_plan);
    free_scan(r->current_scan);

    r->current_plan =  create_plan(r->parsed_sql->stmts[r->next_stmt]);
    r->current_scan = r->current_plan->open(r->current_plan);
    r->next_stmt++;

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
