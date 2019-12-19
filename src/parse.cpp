#include "clok.hpp"

#include <cstdio>
#include <fstream>

std::unique_ptr<Node> parseResult;
std::string filename;

extern std::FILE *yyin;

int parse() {
    yyin = std::fopen(filename.c_str(), "r");
    parseResult = std::unique_ptr<Node>(new Node(0, NodeType::NONE, {}));

    int result = yyparse();

    PLOGD << "yyparse() returned " << result;

    return result;
}

#include "nodetypenames.h"

std::vector<std::string> typesList;

std::string mapNodeType(NodeType type) {
    if (typesList.size() == 0) {
        std::istringstream is(std::string(reinterpret_cast<char*>(nodetypenames_txt), nodetypenames_txt_len), '\n');
        typesList = std::vector<std::string>(std::istream_iterator<std::string>{is}, std::istream_iterator<std::string>());
    }
    return typesList[static_cast<size_t>(type)].substr(0, typesList[static_cast<size_t>(type)].length() - 1);
}

void recursivePrint(std::unique_ptr<Node>& node, std::ofstream& out, int depth) {
    if (depth > 1) {
        out << std::string(depth - 1, ' ');
    }

    if (depth >= 1) {
        out << '-';
    }

    out << '[' << static_cast<unsigned int>(node->type) << ':' << mapNodeType(node->type) << "] <line:" << node->location.first_line << '-' << node->location.last_line << ", col:" << node->location.first_column << '-' << node->location.last_column << '>';
    switch (node->type) {
        case NodeType::DECL:
        case NodeType::PARAM:
        case NodeType::LIBNAME:
        case NodeType::FILEPATH:
        case NodeType::VALSTR:
        case NodeType::QUALPART:
        case NodeType::EXPRBASIC:
        case NodeType::EXPRASSIG:
            out << " -- " << strings[node->value.valC];
            break;
        case NodeType::RUN:
        case NodeType::VALINT:
            out << " -- " << node->value.valI;
            break;
        case NodeType::VALFLOAT:
            out << " -- " << node->value.valF;
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