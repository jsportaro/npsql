#include <list.h>
#include <parser.h>

struct parsed_sql *parse_sql(char *sql, size_t length)
{
    struct parsed_sql *parsed = malloc(sizeof(struct parsed_sql));

    InitListHead(&parsed->statements);

    parsed->stmts = NULL;
    parsed->error_msg = NULL;
    parsed->unresolved = NULL;
    parsed->error = false; 

    yyscan_t sc;

    yylex_init(&sc);
    yy_scan_bytes(sql, length, sc);
    yyparse(sc, parsed);
    yylex_destroy(sc);

    return parsed;
}
