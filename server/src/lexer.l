%option case-insensitive nounput noinput 
%{
#include <parser.gen.h>
#include <stdio.h>
void yyerror(char *s, ...);
%}

%option reentrant bison-bridge noyywrap

id        [A-Za-z][A-Za-z0-9_]*
int       [0-9]+
blank     [ \t\n]
string    '(\\.|''|[^'\n])*'
operators [-+&~|^/%*(),.;!]  

%%

AND         { return AND;    }
CHAR        { return CHAR;   }
CREATE	    { return CREATE; }
SELECT      { return SELECT; }
FROM        { return FROM;   }
INSERT      { return INSERT; }
INT         { return INT;    }
INTO        { return INTO;   }
OR          { return OR;     }
TABLE       { return TABLE;  }
VALUES      { return VALUES; }
WHERE       { return WHERE;  }

{int}       { yylval->INTNUM = atol(yytext); return INTNUM; }
 
{id}        { yylval->IDENTIFIER = strdup(yytext); return IDENTIFIER; }
{string}    { yylval->STRING = strndup(yytext + 1, strlen(yytext) - 2); return STRING; }

{operators} { return yytext[0]; }
 
"="	        { yylval->OPERATOR = yytext; return COMPARISON; }

{blank}     ;

.           { fprintf(stdout, "lex mystery character '%c'\n", *yytext); }

%%