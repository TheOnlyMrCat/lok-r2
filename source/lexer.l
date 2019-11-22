%{
#include <stdlib.h>
#include "grammar.h"

int backslashSequence(char);
%}

%option noyywrap
%option yylineno
%%

run/[^A-Za-z0-9_]  { column += 3; return RUNK; }
load/[^A-Za-z0-9_] { column += 4; return LOADK; }

[A-Za-z_][A-Za-z0-9_]* { yylval.valC = strdup(yytext); column += strlen(yytext); return ID; }

[+-]?[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?f?                        { yylval.valF = strtod(yytext, NULL); column += strlen(yytext); return FLOAT; } // Decimal floating point with decimal point
[+-]?0[xX][0-9A-Fa-f]+\.[0-9A-Fa-f]+([pP][+-]?[0-9A-Fa-f]+)?f? { yylval.valF = strtod(yytext, NULL); column += strlen(yytext); return FLOAT; } // Hex floating point with decimal point
[+-]?[0-9]+[eE][+-]?[:digit]+f?                                { yylval.valF = strtod(yytext, NULL); column += strlen(yytext); return FLOAT; } // Decimal floating point with exponent
[+-]?0[xX][0-9A-Fa-f]+[pP][+-]?[0-9A-Fa-f]+f?                  { yylval.valF = strtod(yytext, NULL); column += strlen(yytext); return FLOAT; } // Hex floating point with exponent
[+-]?[0-9]+f                                                   { yylval.valF = strtod(yytext, NULL); column += strlen(yytext); return FLOAT; } // Decimal floating point integer
[+-]?0[xX][0-9]+f                                              { yylval.valF = strtod(yytext, NULL); column += strlen(yytext); return FLOAT; } // Hex floating point integer

[+-]?[0-9]+            { yylval.valI = strtol(yytext, NULL, 10); column += strlen(yytext); return INTEGER; } // Decimal integer literal
[+-]?0[xX][0-9A-Fa-f]+ { yylval.valI = strtol(yytext, NULL, 16); column += strlen(yytext); return INTEGER; } // Hex integer literal

\'\\x[0-9A-Fa-f]*\'   { yylval.valI = strtol(yytext + (3 * sizeof(char)), 0, 16); column += strlen(yytext); return CHAR; }
\'\\u[0-9A-Fa-f]{4}\' { yylval.valI = strtol(yytext + (3 * sizeof(char)), 0, 16); column += 8; return CHAR; }
\'\\U[0-9A-Fa-f]{8}\' { yylval.valI = strtol(yytext + (3 * sizeof(char)), 0, 16); column += 12; return CHAR; }
\'\\.\'               { yylval.valI = backslashSequence(yytext[2]); column += 4; return CHAR; }
\'.\'                 { yylval.valI = (long) yytext[1]; column += 3; return CHAR; }

\"[^\"\n]\" { yylval.valC = strdup(yytext); column += strlen(yytext); return STRING; }

::   { column += 2; return DBLCOLON; }
->   { column += 2; return RSARROW; }
"#=" { column += 2; return CONSTEQ; }

<<EOF>>    { return 0; }
[ \t]+     { column += strlen(yytext); }
\n         { column = 1; }
.          { column++; return yytext[0]; }

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
}
