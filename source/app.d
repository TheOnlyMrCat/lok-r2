import std.getopt;

void main(string[] args) {
    GetoptResult opts = getopt(args);

    if (opts.helpWanted) {
        defaultGetoptPrinter("clok: Compiler for the Lok programming language", opts.options);
    }
}
