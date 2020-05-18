%{
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void yyerror(char *s, ...);
void emit(char *s, ...);
int yylex();

%}

%union {
    int int_val;
    char *str_val;
}


%token <str_val> NAME
%token <str_val> STRING
%token <int_val> INTNUM

%token AND
%token AS
%token CHAR
%token CREATE
%token EQUALITY
%token FROM
%token INDEX
%token INT
%token OR
%token SELECT
%token TABLE
%token WHERE

%%

npsql_statments: npsql_statement ';'
    | npsql_statments npsql_statement ';'
    ;

npsql_statement: select_statement { emit("Select"); }
    ;

select_statement: 

%%

void
yyerror(char *s, ...)
{
    extern int yylineno;

    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}

void
emit(char *s, ...)
{
    extern int yylineno;

    va_list ap;
    va_start(ap, s);

    fprintf(stdout, "rpn: ");
    vfprintf(stdout, s, ap);
    fprintf(stdout, "\n");
}