module parsing;

import app;
import nodetypes;

import std.conv;
import std.string;
import std.experimental.logger;

import core.stdc.errno;
import core.stdc.stdio;
import core.stdc.string : strerror;
import core.stdc.stdlib : free;

struct CNode {
    int type;
    ValType value;

    int cCap;
    int cCount;
    CNode** children;

    union ValType {
        long valI;
        double valF;
        char *valC;
    }
}

struct ASTNode {
    NodeType type;
    TKVal value;
    ASTNode[] children;

    alias value this;
}

struct TKVal {
    long valI;
    double valF;
    string valC;
}

struct YYLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
}

string currentFile;

extern(C) {
    extern __gshared CNode* parseResult;
    extern __gshared int yylineno;
    extern __gshared int column;
    extern __gshared YYLTYPE yylloc;

    int yyparse();
    int openFile(const char *filename);
    void allocResult();
    void freeResult();

    void yyerror(const char *p) {
        errorf("%s:%d:%d: %s", currentFile, yylloc.first_line, yylloc.first_column, p.fromStringz);
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

    if (astn.isIntType) astn.valI = cn.value.valI;
    else if (astn.isFloatType) astn.valF = cn.value.valF;
    else if (astn.isStringType) {
        astn.valC = cn.value.valC.fromStringz.idup;
        free(cn.value.valC);
    }
    cn.value.valI = 0;

    astn.children.length = cn.cCount;
    for (int i = 0; i < cn.cCount; i++) {
        astn.children[i] = recursiveConvert(cn.children[i]);
    }
    return astn;
}

bool isStringType(ASTNode node) {
    switch (node.type) {
    case NodeType.DECL:
    case NodeType.PARAM:
    case NodeType.LIBNAME:
    case NodeType.FILEPATH:
    case NodeType.VALSTR:
    case NodeType.QUALPART:
    case NodeType.EXPRBASIC:
    case NodeType.EXPRASSIG:
        return true;
    default: return false;
    }
}

bool isIntType(ASTNode node) {
    switch (node.type) {
    case NodeType.RUN:
    case NodeType.VALINT:
        return true;
    default: return false;
    }
}

bool isFloatType(ASTNode node) {
    switch (node.type) {
    case NodeType.VALFLOAT:
        return true;
    default: return false;
    }
}

string mapEnum(int i) {
    // This is a really hacky way using file imports
    string imported = import("nodetypes.d");
    string[] types = imported[(imported.indexOf('{') + 1)..imported.indexOf('}')].split(',');
    if (i >= types.length) {
        return i.to!string;
    } else {
        return types[i].strip;
    }
}
