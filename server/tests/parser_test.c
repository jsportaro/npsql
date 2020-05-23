#include <parser.h>
#include <sql.h>

#include <stdio.h>
#include <string.h>

void test_no_data_select()
{
    char *sql = "select VARNUM - 2 + 2 * 3 / 4; select 1, 2, 4; select hello;";

	size_t len = strlen(sql);

    struct parsed_sql *result = parse_sql(sql, len);

    traverse_stmts(result, NULL);
    delete_stmts(result);
    
    fprintf(stdout, "Done\n");
}

int main(void)
{   
    test_no_data_select();
}