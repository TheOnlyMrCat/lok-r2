#include "clok.hpp"
#include "program.hpp"

void Program::_findSymbols(std::unique_ptr<Node>& tree, std::vector<IdPart> prefix) {
	for (auto& node : tree->children) {
		if (node->type == NodeType::DECL) symbols.emplace_back(node, false, getContext());
		else if (node->type == NodeType::CLASSDEF || node->type == NodeType::STRUCTDEF) {
			prefix.push_back(std::make_pair(strings[node->value.valC], true));
			_findSymbols(node->children[0], prefix);
			prefix.pop_back();
		}
	}
}

void Program::findSymbols(std::unique_ptr<Node>& tree) {
	_findSymbols(tree, {});
}

ProgramContext& Program::getContext() {

}

void Program::findDeclarations(std::unique_ptr<Node>& tree) {

}