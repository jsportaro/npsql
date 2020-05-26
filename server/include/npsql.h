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

enum column_types
{
    TYPE_INT,
    TYPE_CHAR
};

#define TYPE_INT_SIZE 4

struct column
{
    struct byte_buffer name;
    enum column_types type;
    uint16_t size;
};

struct result_set
{

    bool execution_error;
    struct byte_buffer message;

    bool has_rows;
    vector_type(struct column) columns;
};

struct query_results
{
    int sets_to_return;
    int rows_to_return;

    struct result_set set;
    vector_type(uint8_t) current;

    struct parsed_sql *parsed_sql;
};


int query_engine_init(char *data_file, char *log_file, struct query_engine *query_engine);
struct query_results * submit_query(struct query_engine *query_engine, uint8_t *query, size_t length);
bool get_next_set(struct query_results *results);
bool next_record(struct query_results *results);
void free_results(struct query_results *results);
vector_type(uint8_t) get_row_bytes(struct query_results *results);

#endif
