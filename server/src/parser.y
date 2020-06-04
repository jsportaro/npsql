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

%left COMPARISON
%left '+' '-'
%left '*' '/'

%token INT
%token CREATE
%token TABLE
%token SELECT
%token FROM
%token WHERE
%token AND
%token OR
%token COMPARISON 

%type <struct sql_stmt *> stmt select_stmt create_table_stmt
%type <vector_type(struct expr *)> select_expr_list
%type <vector_type(struct table_ref *)> table_references
%type <vector_type(struct column_def *)> create_col_list
%type <struct table_ref *> table_reference
%type <struct expr *> select_expr
%type <struct expr *> expr
%type <struct expr *> opt_where
%type <struct column_def *> create_def
%type <struct type_def *> data_type
%start stmt_list;

%%

stmt_list: 
    stmt ';'                         { vector_push(parsed->stmts, $1); }
  | stmt_list stmt ';'               { vector_push(parsed->stmts, $2); } 
  ;

stmt: 
    select_stmt                      { $$ = $1; }     
  | create_table_stmt                { $$ = $1; }
;

select_stmt: 
    SELECT select_expr_list          { $$ = new_select($2); }
  | SELECT select_expr_list
    FROM table_references            
    opt_where                        { $$ = new_select_data($2, $4, $5); }
;  

select_expr_list: 
    select_expr                      { $$ = new_expr_list($1);          }
  | select_expr_list ',' select_expr { $$ = append_expr_list($1, $3);   }
;

select_expr:
    expr                             { $$ = $1; }
;

table_references:
    table_reference                  { $$ = new_table_list($1); }
;

table_reference:
    "identifier"                     { $$ = new_table_ref($1); }
;

opt_where:
                                     { $$= NULL; }
  | WHERE expr                       { $$ = $2;  }
;

expr:
    "identifier"                     { $$ = new_term_expr(EXPR_IDENIFIER, $1); }
  | "string"                         { $$ = new_term_expr(EXPR_STRING,    $1); }
  | "integer"                        { $$ = new_term_expr(EXPR_INTEGER,   (const void *)(&$1)); }
;

expr: 
    expr '+' expr                    { $$ = new_infix_expr(EXPR_ADD, $1, $3); }
  | expr '-' expr                    { $$ = new_infix_expr(EXPR_SUB, $1, $3); }
  | expr '*' expr                    { $$ = new_infix_expr(EXPR_MUL, $1, $3); }
  | expr '/' expr                    { $$ = new_infix_expr(EXPR_DIV, $1, $3); }
  | expr COMPARISON expr             { $$ = new_infix_expr(EXPR_COMPARISON, $1, $3); }
;

create_table_stmt:
    CREATE TABLE 
    "identifier" 
    '(' create_col_list  ')'         { $$ = new_create_table($3, $5); }
;

create_col_list: 
    create_def                       { $$ = new_column_def_list($1);          }
  | create_col_list ',' create_def   { $$ = append_column_def_list($1, $3);   } 
;

create_def:
    "identifier" data_type           { $$ = create_column_def($1, $2); }
;

data_type:
    INT                              { $$ = create_type_def(1); }
;

%%

void 
yyerror (yyscan_t *locp, struct parsed_sql *parsed, char const *msg) 
{
  UNUSED(locp);
  parsed->error = true;
  for (size_t i = 0; i < strlen(msg); i++)
  {
    vector_push(parsed->error_msg, msg[i]);
  }
  
  fprintf(stderr, "--> %s\n", msg);
}
