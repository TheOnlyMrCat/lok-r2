#include "clok.hpp"

#include <cstdio>
#include <fstream>

std::unique_ptr<Node> parseResult;
std::string filename;

extern std::FILE *yyin;

int parse() {
    yyin = std::fopen(filename.c_str(), "r");
    parseResult = std::unique_ptr<Node>(new Node(NodeType::NONE, 0, {}));

    int result = yyparse();

    PLOGD << "yyparse() returned " << result;

    return result;
}

void recursivePrint(std::unique_ptr<Node>& node, std::ofstream& out, int depth) {
    if (depth > 1) {
        out << std::string(depth - 1, ' ');
    }

    if (depth >= 1) {
        out << '-';
    }

    out << '[' << node->type << "] <line:" << node->location.first_line << '-' << node->location.last_line << ", col:" << node->location.first_column << '-' << node->location.last_column << '>';
    switch (node->type) {
        case DECL:
        case PARAM:
        case LIBNAME:
        case FILEPATH:
        case VALSTR:
        case QUALPART:
        case EXPRBASIC:
        case EXPRASSIG:
            out << ' ' << strings[node->value.valC];
            break;
        case RUN:
        case VALINT:
            out << ' ' << node->value.valI;
            break;
        case VALFLOAT:
            out << ' ' << node->value.valF;
            break;
        default:
            break;
    }

    out << '\n';

    for (int i = 0; i < node->children.size(); i++) {
        recursivePrint(node->children[i], out, depth + 1);
    }
}

void dumpAST(std::string file) {
    std::ofstream out(file);

    recursivePrint(parseResult, out, 0);
}