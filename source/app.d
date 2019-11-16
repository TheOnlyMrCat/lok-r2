module app;

import parsing;

import std.getopt;
import std.stdio;
import std.string;

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

void printAST(ASTNode node, int depth) {
    for (int i = 0; i < depth; i++) {
        write("-");
    }

    writef("(%d)", node.type);

    if (node.isStringType) {
        writeln(node.valC.fromStringz);
    } else if (node.isIntType) {
        writeln(node.valI);
    } else if (node.isFloatType) {
        writeln(node.valF);
    } else {
        writeln();
    }

    depth++;
    for (int i = 0; i < node.count; i++) {
        printAST(node.children[i], depth);
    }
}
