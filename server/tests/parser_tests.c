#include <parser.h>
#include <sql.h>

#include <stdio.h>
#include <string.h>

void test_no_data_select()
{
    char *sql = "select VARNUM - 2 + 2 * 3 / 4;";
    size_t len = strlen(sql);
    struct parsed_sql *result = parse_sql(sql, len);

    assert(result->error == false);

    free_stmts(result);
    fprintf(stdout, "Done\n");
}

void test_select_list()
{
    char *sql = "select 1 + 1, 1 + age, name, age * age from people;";
	size_t len = strlen(sql);
    struct parsed_sql *result = parse_sql(sql, len);

    assert(result->error == false);

    free_stmts(result);
    fprintf(stdout, "Done\n");
}

void test_select_all_table_ref()
{
    char *sql = "select * from people;";
	size_t len = strlen(sql);
    struct parsed_sql *result = parse_sql(sql, len);

    assert(result->error == false);

    free_stmts(result);
    fprintf(stdout, "Done\n");
}

void test_select_table_ref()
{
    char *sql = "select name from people;";
	size_t len = strlen(sql);
    struct parsed_sql *result = parse_sql(sql, len);

    assert(result->error == false);

    free_stmts(result);
    fprintf(stdout, "Done\n");
}

void test_select_table_ref_where()
{
    char *sql = "select name from people where name = 'heather';";
	size_t len = strlen(sql);
    struct parsed_sql *result = parse_sql(sql, len);
    
    assert(result->error == false);

    free_stmts(result);
    fprintf(stdout, "Done\n");
}

void test_parse_error()
{
    char *sql = "selddect name from people wheddre name = 'heather';";
	size_t len = strlen(sql);
    struct parsed_sql *result = parse_sql(sql, len);

    assert(result->error == true);

    free_stmts(result);
    fprintf(stdout, "Done\n");
}

void test_create_table()
{
    char *sql = "create table new_table ( id int, name char(10) );";
    size_t len = strlen(sql);
    struct parsed_sql *result = parse_sql(sql, len);

    assert(result->error == false);

    free_stmts(result);
    fprintf(stdout, "Done\n");
}

int main(void)
{   
    // test_no_data_select();
    test_select_list();
    // test_select_all_table_ref();
    // test_select_table_ref();
    // test_select_table_ref_where();
    // test_parse_error();
    // test_create_table();
    exit(EXIT_SUCCESS);
}