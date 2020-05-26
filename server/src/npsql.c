#include <common.h>
#include <npsql.h>
#include <networking.h>
#include <parser.h>
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

struct query_results * submit_query(struct query_engine *query_engine, uint8_t *query, size_t length)
{
    struct query_results *results = malloc(sizeof(struct query_results));

    results->parsed_sql = parse_sql((char *)query, length);
    results->current_stmt = 0;
    results->sets_to_return = vector_size(results->parsed_sql->stmts);

    results->rows_to_return = 104;
    results->current = NULL;
    results->current_plan = NULL;
    results->current_scan = NULL;
    uint8_t row[] = { 0x6A, 0x6F, 0x73, 0x65, 0x70, 0x68, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00 };

    for (int i = 0; i < 14; i++)
    {
        vector_push(results->current, row[i]);
    }

    UNUSED(query_engine);
    UNUSED(query);
    UNUSED(length);

    return results;
}

bool has_rows(struct query_results *r)
{
    return r->current_scan->has_rows;
}

bool get_next_set(struct query_results *r)
{
    if (r->sets_to_return == r->current_stmt)
    {
        return false;
    }

    r->current_plan =  create_plan(r->parsed_sql->stmts[r->current_stmt]);
    r->current_scan = r->current_plan->open(r->current_plan);
    vector_free(r->current_columns);

    // struct column column;

    // column.name.bytes = malloc(4);
    // column.name.length = 4;
    // column.type = TYPE_CHAR;
    // column.size = 10;
    // memcpy(column.name.bytes, "name", 4);
    // r->set.columns = NULL;
    // vector_push(r->set.columns, column);

    // column.name.bytes = malloc(3);
    // column.name.length = 3;
    // column.type = TYPE_INT;
    // column.size = 4;
    // memcpy(column.name.bytes, "age", 3);

    // vector_push(r->set.columns, column);

    // r->sets_to_return--;

    return true;
}

void free_results(struct query_results *r)
{
    for (struct column *col = vector_begin(r->current_columns); col != vector_end(r->current_columns); ++col) 
    {
        vector_free(col->name);
    }

    vector_free(r->current_columns);
    free_stmts(r->parsed_sql);
    free(r);
}

bool next_record(struct query_results *r)
{
    if (r->current_scan->next(r->current_scan) == true)
    {
        //  If it has rows, it has columns.  If columns are null,
        //  we need to build them
        
    }

    return false;
}
