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
#include <types.h>
}
%define api.value.type union /* Generate YYSTYPE from these types:  */
%token <long>           INTNUM     "integer"
%token <const char *>   STRING     "string"
%token <const char *>   IDENTIFIER "identifier"
%token <const char *>   OPERATOR   "operator"

%left AND
%left OR
%left EQUALITY
%left '+' '-'
%left '*' '/'

%token AND
%token CHAR
%token COMPARISON 
%token CREATE
%token EQUALITY
%token FROM
%token INT
%token INSERT
%token INTO
%token OR
%token SELECT
%token TABLE
%token VALUES
%token WHERE

%type <struct sql_stmt *> stmt select_stmt create_table_stmt insert_stmt
%type <vector_type(struct expr_ctx *)> select_expr_list
%type <vector_type(struct expr *)> value_list
%type <vector_type(struct table_ref *)> table_references
%type <vector_type(struct column_def *)> create_col_list
%type <struct table_ref *> table_reference
%type <struct expr *> select_expr
%type <struct expr *> expr
%type <struct expr *> opt_where
%type <struct column_def *> create_def
%type <struct type_def *> data_type
%type <vector_type(char *)> column_list
%start stmt_list;

%%

stmt_list: 
    stmt ';'                         { vector_push(parsed->stmts, $1); }
  | stmt_list stmt ';'               { vector_push(parsed->stmts, $2); } 
  ;

stmt: 
    select_stmt                      { $$ = $1; }     
  | create_table_stmt                { $$ = $1; }
  | insert_stmt                      { $$ = $1; }
;

select_stmt: 
    SELECT select_expr_list          { $$ = new_select($2); }
  | SELECT select_expr_list
    FROM table_references            
    opt_where                        { 
                                       $$ = new_select_data($2, $4, $5, parsed->unresolved); 
                                       parsed->unresolved = NULL;
                                     }
;  

select_expr_list: 
    select_expr                      { 
                                       $$ = new_expr_ctx_list($1); 
                                     }

  | select_expr_list ',' select_expr { 
                                       $$ = append_expr_ctx_list($1, $3); 
                                     }

  | '*'                              { $$ = new_expr_ctx_list(NULL); }
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
                                     { $$ = NULL; }
  | WHERE expr                       { 
                                       $$ = $2;  
                                     }
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
    INT                              { $$ = create_type_def(TYPE_INT, 4); }
  | CHAR '(' "integer" ')'           { $$ = create_type_def(TYPE_CHAR, $3); }
;

insert_stmt:
    INSERT INTO "identifier" 
    '(' column_list ')'
    VALUES
    '(' value_list ')'               { 
                                       $$ = new_insert($3, $5, $9); }
;

column_list:
     "identifier"                    { $$ = new_column_list($1);          }
   | column_list ',' "identifier"    { $$ = append_column_list($1, $3);   }
;

value_list: 
    expr                             { 
                                       $$ = new_expr_list($1);
                                     }
  | value_list ',' expr              { 
                                       $$ = append_expr_list($1, $3);
                                     }
;

expr:
    "string"                         { $$ =  (struct expr *)new_term_expr(EXPR_STRING,    $1); }
  | "integer"                        { $$ =  (struct expr *)new_term_expr(EXPR_INTEGER,   (const void *)(&$1)); }
  | "identifier"                     { 
                                        struct term_expr *t = new_term_expr(EXPR_IDENIFIER, $1); 
                                        vector_push(parsed->unresolved, t->value.string);  
                                        $$ = (struct expr *)t; 
                                     }
;

expr: 
    expr '+' expr                    { $$ = new_infix_expr(EXPR_ADD, $1, $3); }
  | expr '-' expr                    { $$ = new_infix_expr(EXPR_SUB, $1, $3); }
  | expr '*' expr                    { $$ = new_infix_expr(EXPR_MUL, $1, $3); }
  | expr '/' expr                    { $$ = new_infix_expr(EXPR_DIV, $1, $3); }
  | expr EQUALITY expr               { $$ = new_infix_expr(EXPR_EQU, $1, $3); }
  | expr AND expr                    { $$ = new_infix_expr(EXPR_AND, $1, $3); }
  | expr OR expr                     { $$ = new_infix_expr(EXPR_OR , $1, $3); }
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
