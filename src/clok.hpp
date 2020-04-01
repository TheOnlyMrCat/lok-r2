#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <plog/Log.h>

#include "types.hpp"

typedef std::string IdPart;

extern std::vector<std::string> strings;

strings_t getString(std::string string);
std::string parseFilename(NodePtr& node);

int parse();
void dumpAST(std::unique_ptr<Node>& root, std::string file);
