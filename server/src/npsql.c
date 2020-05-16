#include <common.h>
#include <npsql.h>
#include <networking.h>

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
    results->parse_error = false;


    results->parse_message.bytes = NULL;
    results->parse_message.length = 0;

    results->sets_to_return = 1;
    results->rows_to_return = 104;
    results->current = NULL;
    results->set.execution_error = false;
    uint8_t row[] = { 0x6A, 0x6F, 0x73, 0x65, 0x70, 0x68, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00 };

    for (int i = 0; i < 14; i++)
    {
        VECTOR_PUSH(results->current, row[i]);
    }

    UNUSED(query_engine);
    UNUSED(query);
    UNUSED(length);

    return results;
}


bool get_next_set(struct query_results *results)
{
    if (results->sets_to_return == 0)
    {
        return false;
    }

    results->set.has_rows = true;
    results->set.message.bytes = NULL;
    results->set.message.length = 0;
    struct column column;

    column.name.bytes = malloc(4);
    column.name.length = 4;
    column.type = TYPE_CHAR;
    column.size = 10;
    memcpy(column.name.bytes, "name", 4);
    results->set.columns = NULL;
    VECTOR_PUSH(results->set.columns, column);

    column.name.bytes = malloc(3);
    column.name.length = 3;
    column.type = TYPE_INT;
    column.size = 4;
    memcpy(column.name.bytes, "age", 3);

    VECTOR_PUSH(results->set.columns, column);

    results->sets_to_return--;

    return true;
}

void free_results(struct query_results *results)
{
    for (struct column *col = VECTOR_BEGIN(results->set.columns); col != VECTOR_END(results->set.columns); ++col) 
    {
        free(col->name.bytes);
    }

    VECTOR_FREE(results->set.columns);
    free(results);
}

bool next_record(struct query_results *results)
{
    if (results->rows_to_return < 0)
    {
        return false;
    }
    results->rows_to_return--;
    return true;
}
