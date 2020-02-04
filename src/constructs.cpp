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
			basic = val::value_ptr<SingleType>(SingleType(node, pc));
			break;
		case NodeType::TYPEMULTI:
			typeType = 1;
			tuple = val::value_ptr<TupleType>(TupleType(node, pc));
			break;
		case NodeType::TYPEFN:
			typeType = 2;
			func = val::value_ptr<ReturningType>(ReturningType(node, pc));
			break;
		default:
			PLOGF << "I sincerely hope that this never happens";
			exit(1);
	}
}

Type::Type(SingleType t): typeType(0), basic(t) {}
Type::Type(TupleType t): typeType(1), tuple(t) {}
Type::Type(ReturningType t): typeType(2), func(t) {}

Type::~Type() = default;

Type::Type(const Type&) = default;
Type::Type(Type&&) = default;
Type& Type::operator=(Type&&) = default;

// Node is expected to be of type QUALID
Identifier::Identifier(NodePtr& node, ProgramContext& context) : parts(context.currentNamespace) {
	NodePtr *part;
    for (part = &node->children[0]; (*part)->children.size() > 0; part = &(*part)->children[0]) {
        parts.push_back({strings[(*part)->value.valC], false}); //TODO: Resolve types
    }
	parts.push_back({strings[(*part)->value.valC], false});
}

Identifier::Identifier(std::vector<IdPart> parts): parts(parts) {}

TypeQualifier::TypeQualifier(NodePtr& node) {
	isPointer = node->type == NodeType::TYPEQUALPTR;
	if (node->type == NodeType::TYPEQUALARR) arraySize = node->value.valI;
	if (node->children[0]->type != NodeType::NONE) nested = val::value_ptr<TypeQualifier>(node->children[0]);
}

TypeQualifier::TypeQualifier(bool b, int i): isPointer(b), arraySize(i) {}
TypeQualifier::TypeQualifier(bool b, int i, TypeQualifier f): isPointer(b), arraySize(i), nested(f) {}

SingleType::SingleType(NodePtr& node, ProgramContext& pc):
	id(node->children[0], pc),
	qualifier(node->children[1]->type != NodeType::NONE ? val::value_ptr<TypeQualifier>(node->children[1]) : val::value_ptr<TypeQualifier>(nullptr))
{}

SingleType::SingleType(Identifier i, TypeQualifier q): id(i), qualifier(q) {}
SingleType::SingleType(Identifier i): id(i) {}

TupleType::TupleType(NodePtr& node, ProgramContext& pc) {
    for (int i = 0; i < node->children.size(); i++) {
        types.push_back(Type(node->children[i], pc));
    }
}

TupleType::TupleType(std::vector<Type> ts): types(ts) {}

ReturningType::ReturningType(NodePtr& node, ProgramContext& pc): input(node->children[0], pc), output(node->children[1], pc) {}
ReturningType::ReturningType(TupleType i, Type o): input(i), output(o) {}

Symbol::Symbol(NodePtr& node, bool isType, ProgramContext& pc): id(combineParts(pc.currentNamespace, {strings[node->value.valC], isType})) {
	if (node->children[0]->type == NodeType::TYPESINGLE || node->children[0]->type == NodeType::TYPEMULTI || node->children[0]->type == NodeType::TYPEFN) {
		type = Type(node->children[0], pc);
	}
}

Symbol::Symbol(Type t, Identifier i): type(t), id(i) {}

Expr::Expr(Type t): type(t) {}
Expr::~Expr() = default;

OpExpr::OpExpr(Type t, Expr *l, Expr *r, std::string o): Expr(t), left(std::move(l)), right(std::move(r)), op(o) {}
OpExpr::~OpExpr() {
	delete left;
	delete right;
}

IntValue::IntValue(long long val, int size): Expr(SingleType(Identifier({{"bit", false}}), TypeQualifier(false, size))), value(val) {}
FloatValue::FloatValue(double val, int size): Expr(SingleType(Identifier({{"bit", false}}), TypeQualifier(false, size))), value(val) {}

std::string Symbol::toLokConv() {
	//TODO
	std::string sb;
	for (auto i : id.parts) {
		sb += '_';
		sb += i.first;
	}
	return sb;
}