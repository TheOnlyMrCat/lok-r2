%{
extern int yyerror(const char *p);
%}

%union {
    int valI;
    float valF;
    char *valC;
}

%token <valC> ID

%token LBRACE RBRACE LPAR RPAR LBRACKET RBRACKET COLON

%%
run:
