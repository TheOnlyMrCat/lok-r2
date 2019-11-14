module parse;

import std.string;
import std.experimental.logging;

struct ASTNode {
    int type;

    ASTNode* children;
}

extern(C) int yyparse();

extern(C) int yyerror(const char *p) {
    string s = p.fromStringz;
    error(s);
}
