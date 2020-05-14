%{
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
%}

%union {
    int int_val;
    double float_val;
    char *string_val;
    int subtok;
}

void emit(char *s, ...);

%%

npsql_statments: npsql_statement ';'
    | npsql_statments npsql_statement ';'
    ;

npsql_statement: select_statement { emit("Select"); }
    ;

select_statement: 

%%

emit(char *s, ...)
{
    extern yylineno;

    va_list ap;
    va_start(ap, s);

    fprintf(stdout, "rpn: ");
    vfprintf(stdout, s, ap);
    fprintf(stdout, "\n");
}