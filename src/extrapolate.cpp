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

// node is expected to be one of the EXPR node types
std::unique_ptr<Expr> Program::_extrapolate(std::unique_ptr<Node>& node) {
	switch (node->type) {
		case NodeType::EXPRBASIC:
			//TODO type checking
			return std::make_unique<OpExpr>(_extrapolate(node->children[0]), _extrapolate(node->children[1]), strings[node->value.valC]);
	}
}

void Program::extrapolate(std::unique_ptr<Node>& tree) {
	for (auto& node : tree->children) {
		if (node->type == NodeType::DECL) {
			Type expectedType;
			int expressionIndex = 0;
			if (node->children[0]->type == NodeType::TYPESINGLE || node->children[0]->type == NodeType::TYPEMULTI || node->children[0]->type == NodeType::TYPEFN) {
				expectedType = Type(node->children[0], context);
				expressionIndex = 1;
			}
			_extrapolate(node->children[expressionIndex]);
		} else if (node->type == NodeType::NAMESPACE) {
			context.currentNamespace = Identifier(node->children[0], context).parts;
			extrapolate(node->children[1]);
			context.currentNamespace.clear();
		}
	}
}