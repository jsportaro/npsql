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

%type <int_val> select_expr_list table_list

%start npsql_statments

%%

npsql_statments: npsql_statement ';'
    | npsql_statments npsql_statement ';'
    | ';'
    ;

npsql_statement: 
    select_statement { emit("STATEMENT"); }
    ;

select_statement: 
      SELECT select_expr_list  { emit("SELECTNODATA %d", $2); }
    | SELECT select_expr_list  
      FROM table_list          { emit("SELECT"); } ;
    ;

select_expr_list: 
      select_expr { $$ = 1; }
    | select_expr_list ',' select_expr {$$ = $1 + 1; }
    | '*' { emit("SELECTALL"); $$ = 1; }
    ;

table_list:
      table_reference { $$ = 1; }
    | table_list ',' table_reference {$$ = $1 + 1; }
    ;

select_expr: expr;

table_reference:
      NAME { emit("TABLE %s", $1); free($1); }

expr: NAME { emit("COLUMN %s", $1); free($1); }
    ;

%%

void
emit(char *s, ...)
{
  va_list ap;
  va_start(ap, s);

  printf("rpn: ");
  vfprintf(stdout, s, ap);
  printf("\n");
}

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
