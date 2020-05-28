#include <npsql.h>
#include <plans.h>
#include <scans.h>
#include <stdio.h>
#include <string.h>

int test_if_select_can_do_math(void)
{
    char *sql = "select 1 + 1, 9 * 1;";
    int expected[2] = { 2, 9 };
    struct query_results *results = results = submit_query(NULL, sql, strlen(sql));

    if (results->parsed_sql->error == true)
    {
        goto cleanup;
    }

    while (get_next_set(results))
    {
        while (next_record(results))
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

int main(void)
{
    test_if_select_can_do_math();
}