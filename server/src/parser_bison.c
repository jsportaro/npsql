#include <parser.h>

struct parsed_sql *parse_sql(char *sql, size_t length)
{
    struct parsed_sql *parsed = malloc(sizeof(struct parsed_sql));

	parsed->stmts = NULL;

	yyscan_t sc;
	int res;
	
	yylex_init(&sc);
	yy_scan_bytes(sql, length, sc);
	res = yyparse(sc, parsed);
	yylex_destroy(sc);

    UNUSED(res);

    return parsed;
}
