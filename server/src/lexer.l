%option noyywrap nodefault yylineno case-insensitive nounput noinput
%{

#include <parser.h>
#include <npsql.h>
#include <stdarg.h>
#include <string.h>

void yyerror(char *s, ...);

int old_state;

%}

%% 

AND    { return AND;    }
AS     { return AS;     }
CHAR   { return CHAR;   }
CREATE { return CREATE; }
FROM   { return FROM;   }
INDEX  { return INDEX;  }
INT    { return INT;    }
OR     { return OR;     }
SELECT { return SELECT; }
TABLE  { return TABLE;  }
WHERE  { return WHERE;  }

-?[0-9]+               { yylval.int_val = atoi(yytext); return INTNUM; }
\"(\\.|\"\"|[^"\n])*\" { yylval.str_val = strdup(yytext); return STRING; } 

[=+*/()] { return yytext[0]; }

"=" { return EQUALITY; }

[A-Za-z][A-Za-z0-9] { yylval.str_val = strdup(yytext); return NAME; }

[ \t\n] /* ignore whitespaces */
.       { yyerror("Parsing error '%c'", *yytext); }

%%