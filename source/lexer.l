%{
#include "grammar.h"
%}

%option noyywrap
%%

[:alpha:][:alnum:]* { yylval.valC = yytext; return ID; }

[0-9]+(\.[0-9]+)

"{" { return LBRACE; }
"}" { return RBRACE; }
"[" { return LBRACKET; }
"]" { return RBRACKET; }
"(" { return LPAR; }
")" { return RPAR; }
