#include "clok.hpp"

#include <cstdio>
#include <fstream>

std::unique_ptr<Node> parseResult;
std::string filename;

extern std::FILE *yyin;

int parse() {
    yyin = std::fopen(filename.c_str(), "r");
    parseResult = std::unique_ptr<Node>(new Node(NodeType::NONE, 0));

    int result = yyparse();

    PLOGD << "yyparse() returned " << result;

    return result;
}

