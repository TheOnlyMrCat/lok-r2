module parsing;

import app;
import nodetypes;

import std.string;
import std.experimental.logger;

import core.stdc.errno;
import core.stdc.stdio;
import core.stdc.string : strerror;

struct CNode {
    int type;
    TKVal value;

    int cCap;
    int cCount;
    CNode** children;
}

struct ASTNode {
    NodeType type;
    TKVal value;
    ASTNode[] children;

    alias value this;
}

union TKVal {
    long valI;
    double valF;
    char *valC;
}

string currentFile;

extern(C) {
    extern __gshared CNode* parseResult;
    extern __gshared int yylineno;
    extern __gshared int column;

    int yyparse();
    int openFile(const char *filename);
    void allocResult();
    void freeResult();

    void yyerror(const char *p) {
        errorf("%s:%d:%d: %s", currentFile, yylineno, column, p.fromStringz);
    }

    void doLog(const char *p) {
        warningf(p.fromStringz.idup);
    }
}

void parse(string file) {
    trace("Opening file");
    if (openFile(file.toStringz) != 0) {
        errorf("Failed to open file: %s", strerror(errno));
        return;
    }

    trace("Beginning parse");
    allocResult();
    currentFile = file;
    if (yyparse() == 0) {
        info("Parsing succeeded");
    } else {
        info("Parsing failed");
    }
    printAST(recursiveConvert(parseResult), 0);
    freeResult();
}

ASTNode recursiveConvert(CNode* cn) {
    ASTNode astn;
    astn.type = cast(NodeType) cn.type;
    astn.value = cn.value;

    astn.children.length = cn.cCount;
    for (int i = 0; i < cn.cCount; i++) {
        astn.children[i] = recursiveConvert(cn.children[i]);
    }
    return astn;
}

bool isStringType(ASTNode node) {
    switch (node.type) {
    case NodeType.FUNCDEC:
    case NodeType.PARAM:
    case NodeType.LIBNAME:
    case NodeType.FILEPATH:
    case NodeType.TYPESINGLE:
        return true;
    default: return false;
    }
}

bool isIntType(ASTNode node) {
    switch (node.type) {
    case NodeType.RUN:
        return true;
    default: return false;
    }
}

bool isFloatType(ASTNode node) {
    return false;
}
