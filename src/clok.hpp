#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <plog/Log.h>

#include "grammar.hpp"

typedef std::pair<std::string, bool> IdPart;

extern std::unique_ptr<Node> parseResult;
extern std::vector<std::string> strings;

strings_t getString(std::string string);

int parse();
void dumpAST(std::unique_ptr<Node>& root, std::string file);
