#pragma once

#include <string>
#include <memory>
#include "grammar.hpp"

class Bridge {
public:
    Bridge();

    yy::location location;
    yy::parser parser;
    std::unique_ptr<Node> result;

    int parse(std::string &file);
};

#define YY_DECL yy::parser::symbol_type yylex(Bridge& bridge)
YY_DECL;
