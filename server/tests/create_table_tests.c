#include <assert.h>
#include <buffers.h>
#include <buffer_manager.h>
#include <data_file.h>
#include <data_page.h>
#include <create_table.h>
#include <file.h>
#include <lock_table.h>
#include <heap_table.h>
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
    char *sql = "create table new_table ( id int, name char(10) );";
    size_t len = strlen(sql);
    struct parsed_sql *psql = parse_sql(sql, len);
    
    const char *data_path = "create_table.dat";
    const char *log_path = "create_table.log";
    struct transaction_context ctx = { 0 };
    struct syscat cat = { 0 };

    file_delete(data_path);
    file_delete(log_path);

    initialize_transaction_context(&ctx, data_path, log_path);

    struct transaction *tsx = begin_transaction(&ctx);
    {
        create_system_catalogs(&cat, tsx);

        bool result = execute_create_table(tsx, &cat, (struct create_table *)psql->stmts[0]);

        if (result == true)
        {
            commit(tsx);
        }
        else
        {
            rollback(tsx);
        }
        
    }


    return EXIT_SUCCESS;
}