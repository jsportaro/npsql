#ifndef __NPSQL_H__
#define __NPSQL_H__

#include <common.h>
#include <vector.h>
#include <parser.h>
#include <sql.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DB_OK 1

struct query_engine
{
    char *database;
};



#define TYPE_INT_SIZE 4


struct query_results
{
    uint32_t sets_to_return;
    int rows_to_return;

    vector_type(uint8_t) current;

    struct parsed_sql *parsed_sql;

    uint32_t current_stmt;
    struct plan *current_plan;
    struct scan *current_scan;
    vector_type(struct column) current_columns;
};


int query_engine_init(char *data_file, char *log_file, struct query_engine *query_engine);
struct query_results * submit_query(struct query_engine *query_engine, uint8_t *query, size_t length);
bool get_next_set(struct query_results *results);
bool has_rows(struct query_results *r);
bool next_record(struct query_results *results);
void free_results(struct query_results *results);
vector_type(uint8_t) get_row_bytes(struct query_results *results);

#endif
