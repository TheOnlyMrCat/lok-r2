#pragma once

#include <string>
#include <memory>
#include <vector>

#include <plog/Log.h>

#include "grammar.hpp"

extern std::unique_ptr<Node> parseResult;
extern std::vector<std::string> strings;

strings_t getString(std::string string);

int parse();
void dumpAST(std::unique_ptr<Node>& root, std::string file);

class Decl {
public:
	virtual ~Decl();
};

typedef std::vector<std::unique_ptr<Decl>> Program;
Program createProgram(std::unique_ptr<Node>& syntax);
