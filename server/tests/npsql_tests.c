#include <npsql.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    char *sql = "select 1 + 1, 1 + 1;";
    struct query_results * r = submit_query(NULL, sql, strlen(sql));

    get_next_set(r);
    next_record(r);

    return 0;
}