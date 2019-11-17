module parsing;

import app;
import nodetypes;

import std.string;
import std.experimental.logger;

import core.stdc.stdio;

struct ASTNode {
    int type;
    TKVal value;

    ASTNode* children;
    int cCap;
    int cCount;

    alias value this;
}

union TKVal {
    long valI;
    double valF;
    char *valC;
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

bool isStringType(ASTNode node) {
    return true;
}

bool isIntType(ASTNode node) {
    return true;
}

bool isFloatType(ASTNode node) {
    return true;
}
