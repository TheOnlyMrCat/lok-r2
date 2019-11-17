%code top {
#include <string.h>
#include <stdlib.h>

extern int yyerror(const char *p);
}

%code requires {
  struct ASTNode;
}

%union {
  long long valI;
  double valF;
  char *valC;
  struct ASTNode *valN;
}

%code {
/* Yes, this d file is being included. This is the easiest way I could think of
 * to define the node types once only. The only other way would be to run the
 * parsing file through a C preprocessor, which has obvious downsides.
 */
#include "nodetypes.d"
;
/* Semicolon to avoid a compile error with the enum delcaration for C. Not in
 * the D file because I hate warnings
 */

struct ASTNode {
    int type;
    YYSTYPE value;

    struct ASTNode *children;
    int cCap;
    int cCount;
};

typedef struct ASTNode Node;
}

%define parse.error verbose
%define parse.lac full

%token <valC> ID STRING
%token <valF> FLOAT
%token <valI> INTEGER CHAR

%token DBLCOLON "::" RSARROW "->" CONSTEQ "=!" RUNK "run" LOADK "load"

%type <valC> LibraryName FilePath BasicDeclarator TypeName
%type <valN> LoadExpression FileLocator NamespaceItem RunDeclaration
%type <valN> FuncDeclaration FuncDefinition FunctionBody ParameterList Parameters Parameter
%type <valN> Type SingleType TupleTypes TupleType FunctionType
%type <valN> BlockStatement

%code {
  Node *nalloc(int children, int type) {
    Node *n = (Node*) malloc(sizeof(Node));
    n->type = type;
    n->children = (Node*) malloc(sizeof(Node) * children);
    n->cCap = children;
    return n;
  }

  Node *addChild(Node *parent, Node *child) {
    if (parent->cCount >= parent->cCap) {
      parent->children = realloc(parent->children, parent->cCap * 2);
    }
    parent->children[++parent->cCount] = *child;
    free(child);
    return &parent->children[parent->cCount];
  }
}

%%
top:
    %empty
  | LoadExpression top
  | NamespaceItem
  ;

LoadExpression:
    "load" '(' FileLocator[file] ')' { $$ = nalloc(1, LOAD); addChild($$, $file); }
  ;

FileLocator:
    FilePath[file]                      { $$ = nalloc(1, NONE); addChild($$, nalloc(0, NONE))->value.valC = $file; }
  | LibraryName[lib] ':' FilePath[file] { $$ = nalloc(2, NONE); addChild($$, nalloc(0, NONE))->value.valC = $lib; addChild($$, nalloc(0, NONE))->value.valC = $file; }
  ;

FilePath:
    ID
  | ID '/' FilePath { $$ = realloc($1, sizeof(char) * (strlen($1) + 1 + strlen($3))); strcat($$, "/"); strcat($$, $3); }
  | ID '.' FilePath { $$ = realloc($1, sizeof(char) * (strlen($1) + 1 + strlen($3))); strcat($$, "."); strcat($$, $3); }
  ;

LibraryName:
    ID
  ;

NamespaceItem:
    FuncDeclaration
    RunDeclaration
  ;

FuncDeclaration:
    BasicDeclarator FunctionType "=!" FuncDefinition { $$ = nalloc(2, FUNCDEC); $$->value.valC = $1; addChild($$, $2); addChild($$, $4); }
  | BasicDeclarator FunctionType '=' FuncDefinition  { $$ = nalloc(2, FUNCDEC); $$->value.valC = $1; addChild($$, $2); addChild($$, $4); }
  | BasicDeclarator FunctionType ';'                 { $$ = nalloc(1, FUNCDEC); $$->value.valC = $1; addChild($$, $2); }
  ;

RunDeclaration:
    "run" "=!" FuncDefinition[func]         { $$ = nalloc(1, RUN); addChild($$, $func); $$->value.valI = 0; }
  | "run" INTEGER "=!" FuncDefinition[func] { $$ = nalloc(1, RUN); addChild($$, $func); $$->value.valI = $2; }
  ;

FuncDefinition:
    Parameters FunctionBody { $$ = nalloc(2, FUNCDEF); addChild($$, $2); addChild($$, $1); }
  | FunctionBody            { $$ = nalloc(1, FUNCDEF); addChild($$, $1); }
  ;

FunctionBody:
    BlockStatement
  ;

Parameters:
    '(' ParameterList ')' { $$ = $2; }
  ;

ParameterList:
    Parameter                   { $$ = nalloc(1, PARAMLIST); addChild($$, $1); }
  | ParameterList ',' Parameter { $$ = $1; addChild($$, $3); }
  ;

Parameter:
    ID                         { $$ = nalloc(0, PARAM); $$->value.valC = $1; }
  | BasicDeclarator Type { $$ = nalloc(1, PARAM); $$->value.valC = $1; addChild($$, $2); }
  ;

TypeName:
    ID
  ;

Type:
    SingleType
  | TupleType
  | FunctionType
  ;

SingleType:
    TypeName { $$ = nalloc(0, TYPESINGLE); $$->value.valC = $1; }
  ;

TupleType:
    '(' TupleTypes ')' { $$ = $2; }
  ;

TupleTypes:
    Type            { $$ = nalloc(1, TYPEMULTI); addChild($$, $1); }
  | TupleTypes Type { $$ = $1; addChild($$, $2); }
  ;

FunctionType:
    TupleType "->" Type { $$ = nalloc(2, TYPEFN); addChild($$, $1); addChild($$, $3); }
  ;

BasicDeclarator:
    ID ':' { $$ = $1; }
  ;

BlockStatement:
    '{' '}' { $$ = nalloc(0, BLOCK); }
  ;