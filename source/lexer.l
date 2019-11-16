%{
#include <stdlib.h>
#include "grammar.h"

extern int findMultichar(char*);

int backslashSequence(char);
%}

%option noyywrap
%%

run/[^[:alnum:]_]  { return findMultichar("run"); }
load/[^[:alnum:]_] { return findMultichar("load"); }

[[:alpha:]_][[:alnum:]_]* { yylval.valC = yytext; return ID; }

[+-]?[:digit:]+\.[:digit:]+([eE][+-]?[:digit]+)?f?          { yylval.valF = strtod(yytext, NULL); return FLOAT; } // Decimal floating point with decimal point
[+-]?0[xX][:xdigit:]+\.[:xdigit:]+([pP][+-]?[:xdigit:]+)?f? { yylval.valF = strtod(yytext, NULL); return FLOAT; } // Hex floating point with decimal point
[+-]?[:digit:]+[eE][+-]?[:digit]+f?                         { yylval.valF = strtod(yytext, NULL); return FLOAT; } // Decimal floating point with exponent
[+-]?0[xX][:xdigit:]+[pP][+-]?[:xdigit:]+f?                 { yylval.valF = strtod(yytext, NULL); return FLOAT; } // Hex floating point with exponent
[+-]?[:digit:]+f                                            { yylval.valF = strtod(yytext, NULL); return FLOAT; } // Decimal floating point integer
[+-]?0[xX][:digit:]+f                                       { yylval.valF = strtod(yytext, NULL); return FLOAT; } // Hex floating point integer

[+-]?[:digit:]+       { yylval.valI = strtol(yytext, NULL, 10); return INTEGER; } // Decimal integer literal
[+-]?0[xX][:xdigit:]+ { yylval.valI = strtol(yytext, NULL, 16); return INTEGER; } // Hex integer literal

\'\\x[:xdigit:]*\'   { yylval.valI = strtol(yytext + (3 * sizeof(char)), 0, 16); return CHAR; }
\'\\u[:xdigit:]{4}\' { yylval.valI = strtol(yytext + (3 * sizeof(char)), 0, 16); return CHAR; }
\'\\U[:xdigit:]{8}\' { yylval.valI = strtol(yytext + (3 * sizeof(char)), 0, 16); return CHAR; }
\'\\.\'              { yylval.valI = backslashSequence(yytext[2]); return CHAR; }
\'.\'                { yylval.valI = (long) yytext[1]; return CHAR; }

\"[^\"\n]\" { yylval.valC = yytext; return STRING; }

:: { return findMultichar("::"); }
-> { return findMultichar("->"); }

<<EOF>> { return 0; }
[:space:] // Ignore
. { return yytext[0]; }

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
        case 'v';
            return '\v';
        case '\\':
            return '\\';
        case '\'':
            return '\'';
    }
}
