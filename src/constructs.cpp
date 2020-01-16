#include "type.hpp"

#include "clok.hpp"

Type::Type(NodePtr& node) {
	switch (node->type) {
		case NodeType::TYPESINGLE:
			typeType = 0;
			basic = std::make_unique<SingleType>(node);
			break;
		case NodeType::TYPEMULTI:
			typeType = 1;
			tuple = std::make_unique<TupleType>(node);
			break;
		case NodeType::TYPEFN:
			typeType = 2;
			func = std::make_unique<ReturningType>(node);
			break;
		default:
			PLOGF << "I sincerely hope that this never happens";
			exit(1);
	}
}

Type::~Type() = default;

Type::Type(Type&&) = default;
Type& Type::operator=(Type&&) = default;

Identifier::Identifier(NodePtr& node) {
    for (NodePtr *part = &node->children[0]; (*part)->children.size() > 0; part = &(*part)->children[0]) {
        parts.push_back((*part)->value.valC);
    }
}

TypeQualifier::TypeQualifier(NodePtr& node) {
	isPointer = node->type == NodeType::TYPEQUALPTR;
	if (node->type == NodeType::TYPEQUALARR) arraySize = node->value.valI;
	if (node->children[0]->type != NodeType::NONE) nested = std::make_unique<TypeQualifier>(node->children[0]);
}

SingleType::SingleType(NodePtr& node): id(node->children[0]), qualifier(node->children[1]) {}

TupleType::TupleType(NodePtr& node) {
    for (int i = 0; i < node->children.size(); i++) {
        types.push_back(Type(node->children[i]));
    }
}

ReturningType::ReturningType(NodePtr& node): input(node->children[0]), output(node->children[1]) {}