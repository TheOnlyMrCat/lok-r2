#pragma once

#include "clok.hpp"

#include <string>
#include <vector>
#include <exception>

struct ProgramContext;
struct ReturningType;
struct Identifier;
struct StackFrame;
struct Symbol;

class bad_symbol : public std::runtime_error {
public:
	bad_symbol();
};

template <typename T>
std::vector<T> combineParts(std::vector<T> parts, T suffix) {
	parts.push_back(suffix);
	return parts;
}

ReturningType typeFromFunction(std::unique_ptr<Node>& node, ProgramContext& pc);
std::vector<Symbol> aggreateStack(std::vector<StackFrame> frames);
Symbol getSymbol(Identifier id, ProgramContext& pc);