%{
#include <string.h>

extern int yyerror(const char *p);
%}

%token-table
%define parse.error verbose
%define parse.lac full

%union {
    long valI;
    double valF;
    char *valC;
}

%{
struct ASTNode {
    int type;
    YYSTYPE value;

    struct ASTNode *children;
    int count;
};

typedef struct ASTNode node;
%}

%token <valC> ID STRING
%token <valF> FLOAT
%token <valI> INTEGER CHAR

%%
top:
    %empty
  | LoadExpression top
  | NamespaceItem
  ;

LoadExpression:
    "load" '(' FileLocator ')'
  ;

FileLocator:
    LibraryName ':' FilePath
  ;

FilePath:
    FileName
  | FileName '/' FilePath
  ;

LibraryName:
    ID
  ;

FileName:
    ID
  ;

NamespaceItem:
    FuncDeclaration
    RunDeclaration
  ;

FuncDeclaration:
    BasicDeclarator FunctionType "::" FuncDefinition
  | BasicDeclarator FunctionBody ';'
  ;

RunDeclaration:
    "run" FuncDefinition
  | "run" INTEGER FuncDeclaration
  ;

FuncDefinition:
    Parameters FunctionBody
  | FunctionBody
  ;

FunctionBody:
    BlockStatement
  ;

Parameters:
    '(' ParameterList ')'
  ;

ParameterList:
    Parameter
  | Parameter ParameterList
  ;

Parameter:
    ID
  | BasicDeclarator SingleType
  ;

TypeName:
    ID
  ;

Type:
    SingleType
  | TupleType
  ;

SingleType:
    TypeName
  ;

TupleType:
    '(' TupleTypes ')'
  ;

TupleTypes:
    TypeName
  | TypeName TupleTypes
  ;

FunctionType:
    TupleType "->" Type
  ;

BasicDeclarator:
    ID ':'
  ;

BlockStatement:
    '{' '}'
  ;

%%

int findMultichar(char *token) {
    for (int i = 0; i < YYNTOKENS; i++) {
        if (yytname[i]
        && yytname[i][0] == '"'
        && ! strncmp (yytname[i] + 1, token,
        strlen (token))
        && yytname[i][strlen (token) + 1] == '"'
        && yytname[i][strlen (token) + 2] == 0)
        return i;
    }

    return -1;
}