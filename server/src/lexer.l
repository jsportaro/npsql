%option case-insensitive nounput noinput 
%{
#include <parser.gen.h>
#include <stdio.h>
void yyerror(char *s, ...);
%}

%option reentrant bison-bridge noyywrap

id        [a-zA-Z][a-zA-Z_0-9]*
int       [-+]?[0-9]+
blank     [ \t\n]
string    '(\\.|''|[^'\n])*'
operators [-+&~|^/%*(),.;!]  

%%

SELECT      { return SELECT; }
FROM        { return FROM;   }
WHERE       { return WHERE;  }
AND         { return AND;    }
OR          { return OR;     }
 
{int}       { yylval->INTNUM = atol(yytext); return INTNUM; }
 
{id}        { yylval->IDENTIFIER = yytext; return IDENTIFIER; }
{string}    { yylval->STRING = yytext; return STRING; }

{operators} { return yytext[0]; }
 
"="	        { yylval->OPERATOR = yytext; return COMPARISON; }

{blank}     ;

.           { fprintf(stdout, "lex mystery character '%c'\n", *yytext); }

%%