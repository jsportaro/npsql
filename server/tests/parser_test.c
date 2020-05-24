#include <parser.h>
#include <sql.h>

#include <stdio.h>
#include <string.h>

void test_no_data_select()
{
    char *sql = "select VARNUM - 2 + 2 * 3 / 4;";
    
	size_t len = strlen(sql);

    struct parsed_sql *result = parse_sql(sql, len);

    if (result->has_error == true)
    {
        exit(EXIT_FAILURE);
    }

    free_stmts(result);
    fprintf(stdout, "Done\n");
}

void test_select_table_ref()
{
    char *sql = "select name from people;";

	size_t len = strlen(sql);

    struct parsed_sql *result = parse_sql(sql, len);

    free_stmts(result);
    
    fprintf(stdout, "Done\n");
}

void test_select_table_ref_where()
{
    char *sql = "select name from people where name = 'heather';";

	size_t len = strlen(sql);

    struct parsed_sql *result = parse_sql(sql, len);

    free_stmts(result);
    
    fprintf(stdout, "Done\n");
}

int main(void)
{   
    //test_no_data_select();
    //test_select_table_ref();

    test_select_table_ref_where();
    exit(EXIT_SUCCESS);
}