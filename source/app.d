module app;

import parsing;

import std.getopt;
import std.stdio;
import std.string;
import std.experimental.logger;

void main(string[] args) {
    bool verbose;
    bool vverbose;

    GetoptResult opts = getopt(args,
    "v|verbose", "Print information messages to standard out.", &verbose,
    "loquacious", "Print debug messages to standard out. Intended for developers of the langauge.", &vverbose);

    LogLevel loggerL;
    if (vverbose) {
        loggerL = LogLevel.all;
    } else if (verbose) {
        loggerL = LogLevel.info;
    } else {
        loggerL = LogLevel.warning;
    }

    sharedLog = new class Logger {
        this() {
            super(loggerL);
        }

        override void writeLogMsg(ref LogEntry payload) {
            string formatString;
            switch (payload.logLevel) {
                case LogLevel.trace:
                    formatString = "36";
                    break;
                case LogLevel.info:
                    formatString = "32";
                    break;
                case LogLevel.warning:
                    formatString = "33";
                    break;
                case LogLevel.error:
                    formatString = "91";
                    break;
                case LogLevel.critical:
                    formatString = "31";
                    break;
                case LogLevel.fatal:
                    formatString = "35;1";
                    break;
                default: break;
            }
            writefln("[\x1b[%sm%s\x1b[0m] %s", formatString, payload.logLevel, payload.msg);
        }
    };

    if (opts.helpWanted) {
        defaultGetoptPrinter("clok: Compiler for the Lok programming language", opts.options);
        return;
    }

    if (args.length > 1) {
        foreach (filename; args[1..$]) {
            infof("Parsing file '%s'", filename);
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
        writeln(" \"", node.valC.fromStringz, "\"");
    } else if (node.isIntType) {
        writeln(" ", node.valI);
    } else if (node.isFloatType) {
        writeln(" ", node.valF);
    } else {
        writeln();
    }

    depth++;
    for (int i = 0; i < node.children.length; i++) {
        printAST(node.children[i], depth);
    }
}
