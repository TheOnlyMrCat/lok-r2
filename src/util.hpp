#pragma once

#include "clok.hpp"
#include "type.hpp"

#include <string>
#include <vector>

template <typename T>
std::vector<T> combineParts(std::vector<T> parts, T suffix) {
	parts.push_back(suffix);
	return parts;
}

ReturningType typeFromFunction(std::unique_ptr<Node>& node, ProgramContext& pc);