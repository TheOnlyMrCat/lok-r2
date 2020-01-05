#include "clok.hpp"
#include "location.hh"
#include "bridge.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <iterator>

std::unique_ptr<Node> parseResult;

extern std::FILE *yyin;
extern int yydebug;

Bridge::Bridge() : parser(*this) {}

int Bridge::parse(std::string &filename) {
    yyin = std::fopen(filename.c_str(), "r");
    location.initialize(&filename);
#ifdef DEBUG
	parser.set_debug_level(yydebug);
#endif

    parseResult = std::unique_ptr<Node>(new Node(0, NodeType::NONE, location));
    int result = parser();
    PLOGD << "parse function returned " << result;
    return result;
}

#include "nodetypenames.h"

std::vector<std::string> typesList;

std::string mapNodeType(NodeType type) {
    if (typesList.size() == 0) {
        std::istringstream is(std::string(reinterpret_cast<char*>(nodetypenames_txt), nodetypenames_txt_len));
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

    out << '[' << static_cast<unsigned int>(node->type) << ':' << mapNodeType(node->type) << "] <start:" << node->location.begin.line << ':' << node->location.begin.column << ", end:" << node->location.end.line << '-' << node->location.end.column << '>';
    switch (node->type) {
        case NodeType::DECL:
        case NodeType::PARAM:
        case NodeType::LIBNAME:
        case NodeType::FILEPATH:
        case NodeType::VALSTR:
        case NodeType::QUALPART:
        case NodeType::EXPRBASIC:
        case NodeType::EXPRASSIG:
        case NodeType::ATTR:
            out << " -- " << strings[node->value.valC];
            break;
        case NodeType::RUN:
        case NodeType::VALINT:
        case NodeType::TYPEQUALARR:
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

void dumpAST(std::unique_ptr<Node>& root, std::string file) {
    std::ofstream out(file);

    recursivePrint(root, out, 0);
}