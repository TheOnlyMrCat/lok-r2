#include "clok.hpp"
#include "program.hpp"

void Program::findSymbols(std::unique_ptr<Node>& tree) {
	for (auto& node : tree->children) {
		if (node->type == NodeType::DECL) {
			if (node->children[0]->type == NodeType::CLASSDEF || node->children[0]->type == NodeType::STRUCTDEF) {
				context.currentNamespace.push_back(std::make_pair(strings[node->value.valC], true));
				findSymbols(node->children[0]->children[0]);
				context.currentNamespace.pop_back();
				symbols.emplace_back(node, true, context);
			} else {
				symbols.emplace_back(node, false, context);
			}
			PLOGD << symbols.back().toLokConv();
		} else if (node->type == NodeType::NAMESPACE) {
			context.currentNamespace = Identifier(node->children[0], context).parts;
			findSymbols(node->children[1]);
			context.currentNamespace.clear();
		}
	}
}

void Program::_extrapolate(std::unique_ptr<Node>& node) {
	
}

void Program::extrapolate(std::unique_ptr<Node>& tree) {
	for (auto& node : tree->children) {
		if (node->type == NodeType::DECL) {
			_extrapolate(node);
		} else if (node->type == NodeType::NAMESPACE) {
			context.currentNamespace = Identifier(node->children[0], context).parts;
			extrapolate(node->children[1]);
			context.currentNamespace.clear();
		}
	}
}