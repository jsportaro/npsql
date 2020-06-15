#include <expr_eval.h>
#include <file.h>
#include <npsql.h>
#include <plans.h>
#include <scans.h>
#include <stdio.h>
#include <string.h>

void
setup_db(char *db_name, struct query_engine *engine)
{
    char *data_file = malloc(sizeof(strlen(db_name) + 4));
    strcpy(data_file, db_name);
    strcat(data_file, ".dat");

    char *log_file = malloc(sizeof(strlen(db_name) + 4));
    strcpy(log_file, db_name);
    strcat(log_file, ".log");

    file_delete(data_file);
    file_delete(log_file);

    char *create = "create table people (age int, name char(30));";
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

    char *insert = "insert into people (age, name) values (36, 'Heather');insert into people (age, name) values (37, 'Joe');";
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


int main(void)
{
    struct query_engine engine = { 0 };

    setup_db("plans_test", &engine);

    char *select = "select age, name from people where age = 36;";
    struct query_results *results = submit_query(&engine, select, strlen(select));

    if (results->parsed_sql->error == true)
    {
        goto cleanup;
    }
    struct value v;
    while (get_next_set(results))
    {
        for (size_t i = 0; i < vector_size(results->columns); i++)
        {
            fprintf(stdout, "Column ( %s, %zu, %d ) \n", results->columns[i].name, results->columns[i].size, results->columns[i].type);
        }

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
}