#include <npsql.h>
#include <plans.h>
#include <scans.h>
#include <stdio.h>
#include <string.h>

void 
test_if_select_can_do_math(void)
{
    struct query_engine engine;
    char *sql = "select 1 + 1, 9 * 1;";
    int expected[2] = { 2, 9 };
    query_engine_init("test_npsql.dat", "test_npsql.log", &engine);

    struct query_results *results = results = submit_query(&engine, sql, strlen(sql));

    if (results->parsed_sql->error == true)
    {
        goto cleanup;
    }

    while (get_next_set(results))
    {
        while (next_set_record(results))
        {
            vector_type(struct scan_field) fields = results->current_scan->scan_fields;
            for (size_t i = 0; i < vector_size(fields); i++)
            {
                struct scan_field field = fields[i];
                switch (field.type)
                {
                    case TYPE_INT:
                        assert(expected[i] == field.value.number);
                        break;
                    default:
                        break;
                }
            }
        }
    }

cleanup:
    free_results(results);

    fprintf(stdout, "%s passed\n", __func__);
}

void 
test_if_can_create_table(void)
{
    struct query_engine engine = { 0 };
    char *sql = "create table people ( age int, name char(30));";
    query_engine_init("test_if_can_create_table.dat", "test_if_can_create_table.log", &engine);

    struct query_results *results = results = submit_query(&engine, sql, strlen(sql));

    if (results->parsed_sql->error == true)
    {
        goto cleanup;
    }

    while (get_next_set(results))
    {
        while (next_set_record(results))
        {
            
        }
    }
    
cleanup:
    free_results(results);
    free_query_engine(&engine);
    fprintf(stdout, "%s passed\n", __func__);
}

void 
test_if_can_insert_into(void)
{
    struct query_engine engine = { 0 };
    char *create = "create table people ( age int, name char(30));";
    query_engine_init("test_if_can_insert_into.dat", "test_if_can_insert_into.log", &engine);

    struct query_results *results = results = submit_query(&engine, create, strlen(create));

    if (results->parsed_sql->error == true)
    {
        goto cleanup;
    }

    while (get_next_set(results))
    {
        while (next_set_record(results))
        {
            
        }
    }

    free_results(results);

    char *insert = "insert into people (age, name) values (36, 'Heather');";
    results = submit_query(&engine, insert, strlen(insert));

    if (results->parsed_sql->error == true)
    {
        goto cleanup;
    }

    while (get_next_set(results))
    {
        while (next_set_record(results))
        {
            
        }
    }

cleanup:
    free_results(results);
    free_query_engine(&engine);

    fprintf(stdout, "%s passed\n", __func__);
}

void dbug()
{
    struct transaction tsx;

    tsx.locks = NULL;
    tsx.ctx = malloc(sizeof(struct transaction_context));
    memset(tsx.ctx, 0, sizeof(struct transaction_context));
    create_lock_table(&tsx.ctx->locks);

    get_lock(&tsx, 1, true);
    get_lock(&tsx, 4, true);
    get_lock(&tsx, 4, true);
    get_lock(&tsx, 4, true);
    get_lock(&tsx, 9, true);

    free_lock_table(&tsx.ctx->locks);
    vector_free(tsx.locks);
    free(tsx.ctx);
};

int main(void)
{
    //test_if_select_can_do_math();
    //test_if_can_create_table();
    test_if_can_insert_into();
    //dbug();
}