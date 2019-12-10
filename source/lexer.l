%{
#include <stdlib.h>
#include "grammar.h"

int backslashSequence(char);

#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = column; yylloc.last_column = column + yyleng - 1; \
    column += yyleng;
%}

%option noyywrap
%option yylineno
%%

run/[^A-Za-z0-9_]  { return RUNK; }
load/[^A-Za-z0-9_] { return LOADK; }

[A-Za-z_][A-Za-z0-9_]* { yylval.valC = strdup(yytext); return ID; }

[+-]?[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?f?                        { yylval.valF = strtod(yytext, NULL); return FLOAT; } // Decimal floating point with decimal point
[+-]?0[xX][0-9A-Fa-f]+\.[0-9A-Fa-f]+([pP][+-]?[0-9A-Fa-f]+)?f? { yylval.valF = strtod(yytext, NULL); return FLOAT; } // Hex floating point with decimal point
[+-]?[0-9]+[eE][+-]?[:digit]+f?                                { yylval.valF = strtod(yytext, NULL); return FLOAT; } // Decimal floating point with exponent
[+-]?0[xX][0-9A-Fa-f]+[pP][+-]?[0-9A-Fa-f]+f?                  { yylval.valF = strtod(yytext, NULL); return FLOAT; } // Hex floating point with exponent
[+-]?[0-9]+f                                                   { yylval.valF = strtod(yytext, NULL); return FLOAT; } // Decimal floating point integer
[+-]?0[xX][0-9]+f                                              { yylval.valF = strtod(yytext, NULL); return FLOAT; } // Hex floating point integer

[+-]?[0-9]+            { yylval.valI = strtol(yytext, NULL, 10); return INTEGER; } // Decimal integer literal
[+-]?0[xX][0-9A-Fa-f]+ { yylval.valI = strtol(yytext, NULL, 16); return INTEGER; } // Hex integer literal

\'\\x[0-9A-Fa-f]*\'   { yylval.valI = strtol(yytext + (3 * sizeof(char)), 0, 16); return CHAR; }
\'\\u[0-9A-Fa-f]{4}\' { yylval.valI = strtol(yytext + (3 * sizeof(char)), 0, 16); return CHAR; }
\'\\U[0-9A-Fa-f]{8}\' { yylval.valI = strtol(yytext + (3 * sizeof(char)), 0, 16); return CHAR; }
\'\\.\'               { yylval.valI = backslashSequence(yytext[2]); return CHAR; }
\'.\'                 { yylval.valI = (long) yytext[1]; return CHAR; }

\"[^\"\n]*\" {
    int len = strlen(yytext);
    yylval.valC = malloc(sizeof(char) * (len - 1));
    memcpy(yylval.valC, yytext + 1, len - 2);
    yylval.valC[len - 2] = '\0';
    return STRING;
}

"::" { return DBLCOLON; }
"->" { return RSARROW; }
"||" { return DBLBAR; }
"&&" { return DBLAND; }
"^^" { return DBLXOR; }
"<<" { return DBLLEFT; }
">>" { return DBLRIGHT; }
"++" { return DBLPLUS; }
"--" { return DBLMINUS; }
"!!" { return DBLNOT; }
"==" { return DBLEQ; }
"!=" { return NOTEQ; }
">=" { return GTEQ; }
"<=" { return LTEQ; }
"=>" { return RDARROW; }

"+=" { return COMPADD; }
"-=" { return COMPSUB; }
"*=" { return COMPMUL; }
"/=" { return COMPDIV; }
"%=" { return COMPMOD; }
"&=" { return COMPAND; }
"|=" { return COMPIOR; }
"^=" { return COMPXOR; }
"<<=" { return COMPASL; }
">>=" { return COMPASR; }
">>>=" { return COMPUSR; }

">>>" { return TPLRIGHT; }
">=>" { return RRDARROW; }

<<EOF>>    { return 0; }
[ \t]+     // Whitespace need not be
\n         { column = 1; }
.          { return yytext[0]; }

%%

int backslashSequence(char c) {
    switch (c) {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'e':
            return '\e';
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case 'v':
            return '\v';
        case '\\':
            return '\\';
        case '\'':
            return '\'';
    }
    return '\0';
}
