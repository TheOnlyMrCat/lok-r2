#include "type.hpp"

#include "clok.hpp"
#include "program.hpp"
#include "util.hpp"

Type::Type() {
	typeType = -1;
}

Type::Type(NodePtr& node, ProgramContext& pc) {
	switch (node->type) {
		case NodeType::TYPESINGLE:
			typeType = 0;
			basic = std::make_unique<SingleType>(node, pc);
			break;
		case NodeType::TYPEMULTI:
			typeType = 1;
			tuple = std::make_unique<TupleType>(node, pc);
			break;
		case NodeType::TYPEFN:
			typeType = 2;
			func = std::make_unique<ReturningType>(node, pc);
			break;
		default:
			PLOGF << "I sincerely hope that this never happens";
			exit(1);
	}
}

Type::~Type() = default;

Type::Type(Type&&) = default;
Type& Type::operator=(Type&&) = default;

Identifier::Identifier(NodePtr& node, ProgramContext& context) {
    for (NodePtr *part = &node->children[0]; (*part)->children.size() > 0; part = &(*part)->children[0]) {
        parts.push_back({strings[(*part)->value.valC], false}); //! Requires resolution of types
    }
}

Identifier::Identifier(std::vector<IdPart> parts): parts(parts) {}

TypeQualifier::TypeQualifier(NodePtr& node) {
	isPointer = node->type == NodeType::TYPEQUALPTR;
	if (node->type == NodeType::TYPEQUALARR) arraySize = node->value.valI;
	if (node->children[0]->type != NodeType::NONE) nested = std::make_unique<TypeQualifier>(node->children[0]);
}

SingleType::SingleType(NodePtr& node, ProgramContext& pc):
	id(node->children[0], pc),
	qualifier(node->children[1]->type != NodeType::NONE ? std::make_unique<TypeQualifier>(node->children[1]) : nullptr)
{}

TupleType::TupleType(NodePtr& node, ProgramContext& pc) {
    for (int i = 0; i < node->children.size(); i++) {
        types.push_back(Type(node->children[i], pc));
    }
}

ReturningType::ReturningType(NodePtr& node, ProgramContext& pc): input(node->children[0], pc), output(node->children[1], pc) {}

Symbol::Symbol(NodePtr& node, bool isType, ProgramContext& pc): id(combineParts(pc.currentNamespace, {strings[node->value.valC], isType})) {
	if (node->children[0]->type == NodeType::TYPESINGLE || node->children[0]->type == NodeType::TYPEMULTI || node->children[0]->type == NodeType::TYPEFN) {
		type = Type(node->children[0], pc);
	}
}

std::string Symbol::toLokConv() {
	std::string sb;
	for (auto i : id.parts) {
		sb += '_';
		sb += i.first;
	}
	return sb;
}