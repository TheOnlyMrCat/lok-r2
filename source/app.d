module app;

import parsing;

import std.getopt;

void main(string[] args) {
    GetoptResult opts = getopt(args);

    if (opts.helpWanted) {
        defaultGetoptPrinter("clok: Compiler for the Lok programming language", opts.options);
        return;
    }

    if (args.length > 1) {
        foreach (filename; args[1..$]) {
            parse(filename);
        }
    }
}

void printAST(ASTNode *node, int depth) {

}
