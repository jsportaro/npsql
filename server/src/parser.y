%define api.pure full
%lex-param {void *scanner}
%parse-param {void *scanner}{struct parsed_sql *parsed}

%define parse.trace
%define parse.error verbose

%{
#include <sql.h>
#include <parser.gen.h>
#include <lexer.gen.h>
#include <vector.h>
#include <common.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>


void yyerror (yyscan_t *locp, struct parsed_sql *parsed, char const *msg);
void emit(char *s, ...);
%}

%code requires
{
#include <sql.h>
#include <vector.h>
}
%define api.value.type union /* Generate YYSTYPE from these types:  */
%token <long>           INTNUM     "integer"
%token <const char *>   STRING     "string"
%token <const char *>   IDENTIFIER "identifier"
%token <const char *>   OPERATOR   "operator"

%token SELECT
%token COMPARISON 

%type <struct sql_stmt *> select_stmt
%type <vector_type(struct expr *)> select_expr_list
%type <struct expr *> select_expr
%type <struct expr *> expr factor term

%start stmt;

%%

stmt: 
    select_stmt                      { parsed->sql = $1; }     
;

select_stmt: 
    SELECT select_expr_list          { $$ = new_select($2); }
;  

select_expr_list: 
    select_expr                      { $$ = new_expr_list($1);          }
  | select_expr_list ',' select_expr { $$ = append_expr_list($1, $3);   }
;

select_expr:
    expr         { $$ = $1; }
;

expr: 
     factor
   | expr '+' factor { $$ = new_infix_expr(EXPR_ADD, $1, $3); }
   | expr '-' factor { $$ = new_infix_expr(EXPR_SUB, $1, $3); }
;

factor:
     term
   | factor '*' term { $$ = new_infix_expr(EXPR_MUL, $1, $3); }
   | factor '/' term { $$ = new_infix_expr(EXPR_DIV, $1, $3); }
;

term:
     "identifier"  { $$ = new_term_expr(EXPR_IDENIFIER, $1); }
   | "string"      { $$ = new_term_expr(EXPR_STRING,    $1); }
   | "integer"     { $$ = new_term_expr(EXPR_INTEGER,   (const void *)(&$1)); }
;


%%

void 
yyerror (yyscan_t *locp, struct parsed_sql *parsed, char const *msg) 
{
  UNUSED(locp);
  UNUSED(parsed);
  
	fprintf(stderr, "--> %s\n", msg);
}

void
emit(char *s, ...)
{
  va_list ap;
  va_start(ap, s);

  printf("rpn: ");
  vfprintf(stdout, s, ap);
  printf("\n");
}

