#include "clok.hpp"
#include "program.hpp"

void Program::_findSymbols(std::unique_ptr<Node>& tree) {
	for (auto& node : tree->children) {
		if (node->type == NodeType::DECL) {
			if (node->children[0]->type == NodeType::CLASSDEF || node->children[0]->type == NodeType::STRUCTDEF) {
				context.currentNamespace.push_back(std::make_pair(strings[node->value.valC], true));
				_findSymbols(node->children[0]->children[0]);
				context.currentNamespace.pop_back();
				symbols.emplace_back(node, true, context);
			} else {
				symbols.emplace_back(node, false, context);
			}
			PLOGD << symbols.back().toLokConv();
		}
	}
}

void Program::findSymbols(std::unique_ptr<Node>& tree) {
	_findSymbols(tree);
}

void Program::findDeclarations(std::unique_ptr<Node>& tree) {

}