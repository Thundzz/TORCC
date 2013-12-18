%{
#include "y.tab.h"
#include <string.h>
#include <stdlib.h>
%}

%%
[0-9]+ {yylval.n = atoi(yytext); return N;}
[a-zA-Z]+ {yylval.s = strdup(yytext); return ID;}
[=+*();\-] {return yytext[0];}
[ \t] {}
[\n] {}

%%