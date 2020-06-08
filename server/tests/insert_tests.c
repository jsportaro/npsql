#include <assert.h>
#include <buffers.h>
#include <buffer_manager.h>
#include <data_file.h>
#include <data_page.h>
#include <create_table.h>
#include <file.h>
#include <lock_table.h>
#include <heap_table.h>
#include <insert.h>
#include <parser.h>
#include <storage.h>
#include <syscat.h>
#include <sql.h>
#include <transaction.h>
#include <threads.h>

#include <stdio.h>
#include <string.h>


int main(void)
{
    struct transaction *tsx = NULL;
    char *create_table = "create table new_table ( id int, name char(10) );";
    char *insert_into  = "insert into new_table(id, name) values (1, 'Heather');";

    struct parsed_sql *ct_result = parse_sql(create_table, strlen(create_table));
    struct parsed_sql *ii_result = parse_sql(insert_into, strlen(insert_into));

    const char *data_path = "insert_into.dat";
    const char *log_path = "insert_into.log";
    struct transaction_context ctx = { 0 };
    struct syscat cat = { 0 };

    file_delete(data_path);
    file_delete(log_path);

    initialize_transaction_context(&ctx, data_path, log_path);

    tsx = begin_transaction(&ctx);
    {
        create_system_catalogs(&cat, tsx);

        bool result = execute_create_table(tsx, &cat, (struct create_table *)ct_result->stmts[0]);

        if (result == true)
        {
            commit(tsx);
        }
        else
        {
            rollback(tsx);
        }
        
    }

    tsx = begin_transaction(&ctx);
    {
        bool result = execute_insert_into(tsx, &cat, (struct insert *)ii_result->stmts[0]);

        if (result == true)
        {
            commit(tsx);
        }
        else
        {
            rollback(tsx);
        }
    }

    free_stmts(ct_result);
    free_stmts(ii_result);

    return EXIT_SUCCESS;
}