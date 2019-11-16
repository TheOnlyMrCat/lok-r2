module parsing;

import app;

import std.string;
import std.experimental.logger;

import core.stdc.stdio;

struct ASTNode {
    int type;
    TKVal value;

    ASTNode* children;
    int count;
}

union TKVal {

}

extern(C) {
    extern FILE *yyin;
    extern ASTNode *parseResult;

    int yyparse();

    void yyerror(const char *p) {
        error(p.fromStringz);
    }
}

void parse(string file) {
    yyin = fopen(file.toStringz, "r");
}
