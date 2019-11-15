module parse;

import std.string;
import std.experimental.logger;

struct ASTNode {
    int type;

    ASTNode* children;
    int count;
}

extern(C) int yyparse();

extern(C) void yyerror(const char *p) {
    error(p.fromStringz);
}
