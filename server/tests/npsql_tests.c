#include <expr_eval.h>
#include <file.h>
#include <npsql.h>
#include <plans.h>
#include <scans.h>
#include <stdio.h>
#include <string.h>

void
setup_db(const char *db_name, struct query_engine *engine)
{
    char *data_file = malloc(strlen(db_name) + 5);
    memset(data_file, 0, strlen(db_name) + 5);
    strcpy(data_file, db_name);
    strcat(data_file, ".dat");

    char *log_file = malloc(strlen(db_name) + 5);
    memset(log_file, 0, strlen(db_name) + 5);
    strcpy(log_file, db_name);
    strcat(log_file, ".log");

    file_delete(data_file);
    file_delete(log_file);

    char *create = "create table people (id int, age int, name char(30));";
    query_engine_init(data_file, log_file, engine);

    struct query_results *results = results = submit_query(engine, create, strlen(create));

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

    char *insert = "insert into people (id, age, name) values (1, 36, 'Heather');insert into people (id, age, name) values (2, 37, 'Joe');";
    results = submit_query(engine, insert, strlen(insert));

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
    free(data_file);
    free(log_file);
}


void select_plan_with_data(void)
{
    struct query_engine engine = { 0 };

    setup_db("plans_test", &engine);

    char *select = "select age, name from people where age = 36 and 1 = 1;";
    struct query_results *results = submit_query(&engine, select, strlen(select));

    if (results->parsed_sql->error == true)
    {
        goto cleanup;
    }
    struct value v;
    while (get_next_set(results))
    {
        while (next_set_record(results))
        {
            vector_type(struct plan_column) c = results->columns;

            for (size_t i = 0; i < vector_size(c); i++)
            {
                if (c[i].expr == NULL)
                {
                    results->current_scan->get_value(results->current_scan, c[i].name, &v);
                }
                else
                {
                    v = eval(c[i].expr, results->current_scan);
                }
                
                reset(&v);
            }
        }
    }
cleanup:
    free_query_engine(&engine);
    free_results(results);
}

void select_plan_without_data(void)
{
    struct query_engine engine = { 0 };

    setup_db("plans_test", &engine);

    char *select = "select 1 + 1, 3 * 8;";
    struct query_results *results = submit_query(&engine, select, strlen(select));

    if (results->parsed_sql->error == true)
    {
        goto cleanup;
    }

    struct value v;
    while (get_next_set(results))
    {
        while (next_set_record(results))
        {
            vector_type(struct expr_ctx *) e = get_sql_select(results);

            for (int i = 0; i < vector_size(e); i++)
            {
                
                v = eval(e[i]->expr, results->current_scan);
            }
        }
    }
cleanup:
    free_query_engine(&engine);
    free_results(results);
}

void select_plan_star(void)
{
    struct query_engine engine = { 0 };

    setup_db("plans_test", &engine);

    char *select = "select * from people;";
    struct query_results *results = submit_query(&engine, select, strlen(select));

    if (results->parsed_sql->error == true)
    {
        goto cleanup;
    }
    struct value v;
    while (get_next_set(results))
    {
        while (next_set_record(results))
        {
            vector_type(struct plan_column) c = results->columns;

            for (size_t i = 0; i < vector_size(c); i++)
            {
                if (c[i].expr == NULL)
                {
                    results->current_scan->get_value(results->current_scan, c[i].name, &v);
                }
                else
                {
                    v = eval(c[i].expr, results->current_scan);
                }
                
                reset(&v);
            }
        }
    }
cleanup:
    free_query_engine(&engine);
    free_results(results);
}

int main(void)
{
    select_plan_with_data();
    //select_plan_without_data();
    select_plan_star();
    
    return EXIT_SUCCESS;
}