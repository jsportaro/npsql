#ifndef __PARSER_H__
#define __PARSER_H__

#include <common.h>
#include <sql.h>

#ifdef BISONED
#include <parser.gen.h>
#include <lexer.gen.h>
#else
#error MUST TURN ON BISON FOR NOW
#endif

struct parsed_sql *parse_sql(char *sql, size_t length);

#endif
