#include "type.hpp"

#include "clok.hpp"
#include "program.hpp"
#include "util.hpp"
#include "grammar.hpp"

Type::Type() {
	typeType = -1;
}

Type::Type(NodePtr& node, ProgramContext& pc) {
	switch (node->type) {
		case NodeType::TYPESINGLE:
			typeType = 0;
			basic = SingleType(node, pc);
			break;
		case NodeType::TYPEMULTI:
			typeType = 1;
			tuple = TupleType(node, pc);
			break;
		case NodeType::TYPEFN:
			typeType = 2;
			func = ReturningType(node, pc);
			break;
		default:
			PLOGF << "Unexpected node type when constructing type";
			throw;
	}
}

Type::Type(SingleType t): typeType(0), basic(new SingleType(t)) {}
Type::Type(TupleType t): typeType(1), tuple(new TupleType(t)) {}
Type::Type(ReturningType t): typeType(2), func(new ReturningType(t)) {}

Type::~Type() = default;

Type& Type::operator=(const Type& other) {
	switch (other.typeType) {
		case 0:
			typeType = 0;
			basic = other.basic;
			// tuple = val::value_ptr<TupleType>(nullptr);
			// func = val::value_ptr<ReturningType>(nullptr);
			break;
		case 1:
			typeType = 1;
			// basic = val::value_ptr<SingleType>(nullptr);
			tuple = other.tuple;
			// func = val::value_ptr<ReturningType>(nullptr);
			break;
		case 2:
			typeType = 2;
			// basic = val::value_ptr<SingleType>(nullptr);
			// tuple = val::value_ptr<TupleType>(nullptr);
			func = other.func;
			break;
		default:
			typeType = -1;
			// basic = val::value_ptr<SingleType>(nullptr);
			// tuple = val::value_ptr<TupleType>(nullptr);
			// func = val::value_ptr<ReturningType>(nullptr);
			break;
	}
	return *this;
}

bool Type::operator==(const Type& other) const {
	if (typeType != other.typeType && typeType != -1 && other.typeType != -1) return false;
	switch (typeType) {
		case 0:
			return basic->operator==(*other.basic);
		case 1:
			return tuple->operator==(*other.tuple);
		case 2:
			return func->operator==(*other.func);
		default:
			throw;
	}
}

bool SingleType::operator==(const SingleType& other) const {
	return id == other.id && ((qualifier.get() != nullptr && other.qualifier.get() != nullptr) ? qualifier->operator==(*other.qualifier) : qualifier == other.qualifier);
}

bool TupleType::operator==(const TupleType& other) const {
	return types == other.types;
}

bool ReturningType::operator==(const ReturningType& other) const {
	return input == other.input && output == other.output;
}

bool TypeQualifier::operator==(const TypeQualifier& other) const {
	return isPointer == other.isPointer && forceUpgrade == other.forceUpgrade && arraySize == other.arraySize
		&& (nested.get() != nullptr && other.nested.get() != nullptr) ? nested->operator==(*other.nested) : nested == other.nested;
}

// Node is expected to be a FullyQualifiedPath
Identifier::Identifier(NodePtr& node, bool ignore, ProgramContext& context) : parts(ignore ? std::vector<IdPart>() : context.currentNamespace) {
	Node *part;
    for (part = node->children[0].get(); part->children.size() > 0; part = part->children[0].get()) {
        parts.push_back({strings[part->value.valC], false}); //TODO: Resolve types
    }
	parts.push_back({strings[part->value.valC], false});
}

Identifier::Identifier(std::vector<IdPart> parts): parts(parts) {}

bool Identifier::operator==(const Identifier &other) const {
	return parts == other.parts;
}

bool Identifier::operator<(const Identifier &other) const {
	return parts < other.parts;
}

TypeQualifier::TypeQualifier(NodePtr& node) {
	isPointer = node->type == NodeType::TYPEQUALPTR;
	forceUpgrade = node->type == NodeType::TYPEQUALUPG;
	if (node->type == NodeType::TYPEQUALARR) arraySize = node->value.valI;
	if (node->children[0]->type != NodeType::NONE) nested = val::value_ptr<TypeQualifier>(node->children[0]);
}

TypeQualifier::TypeQualifier(bool b, bool f, int i): isPointer(b), forceUpgrade(f), arraySize(i) {}
TypeQualifier::TypeQualifier(bool b, bool f, int i, TypeQualifier n): isPointer(b), forceUpgrade(f), arraySize(i), nested(n) {}

SingleType::SingleType(NodePtr& node, ProgramContext& pc):
	id(node->children[0], false, pc),
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

ReturningType::ReturningType(NodePtr& node, ProgramContext& pc):
	input(node->children[0], pc),
	output(node->children.size() > 1 ? Type(node->children[1], pc) : Type())
{}

ReturningType::ReturningType(TupleType i, Type o): input(i), output(o) {}

Symbol::Symbol(NodePtr& node, bool isType, ProgramContext& pc): id(combineParts(pc.currentNamespace, {strings[node->value.valC], isType})) {
	if (node->children[0]->type == NodeType::TYPESINGLE || node->children[0]->type == NodeType::TYPEMULTI || node->children[0]->type == NodeType::TYPEFN) {
		type = Type(node->children[0], pc);
	}
}

Symbol::Symbol(Type t, Identifier i): type(t), id(i) {}

//! Requires call of getSymbol twice. Is this fixable?
SymbolExpr::SymbolExpr(Identifier i, ProgramContext &pc): Expr(getSymbol(i, pc).type), symbol(getSymbol(i, pc)) {}

Expr::Expr(Type t): type(t) {}
Expr::~Expr() = default;

OpExpr::OpExpr(Type t, Expr *l, Expr *r, std::string o): Expr(t), left(std::move(l)), right(std::move(r)), op(o) {}
OpExpr::~OpExpr() {
	delete left;
	delete right;
}

IntValue::IntValue(long long val, int size): Expr(SingleType(Identifier({{"bit", true}}), TypeQualifier(false, false, size))), value(val) {}
FloatValue::FloatValue(double val, int size): Expr(SingleType(Identifier({{"bit", true}}), TypeQualifier(false, false, size))), value(val) {}
BitValue::BitValue(bool val): Expr(SingleType(Identifier({{"bit", true}}))), value(val) {}
StringValue::StringValue(std::string val): Expr(SingleType(Identifier({{"bit", true}}), TypeQualifier(false, false, val.length(), TypeQualifier(false, true, 0, TypeQualifier(false, false, 8))))), value(val) {}
FuncValue::FuncValue(ReturningType t, std::vector<Statement*> v): Expr(t), statements(v) {}
FuncValue::~FuncValue() {
	for (auto s : statements) {
		delete s;
	}
}

std::string Symbol::toLokConv() {
	//TODO
	std::string sb;
	for (auto i : id.parts) {
		sb += '_';
		sb += i.first;
	}
	return sb;
}

Value *Statement::codegen() {
	PLOGF << "Unsupported code generation target";
	throw;
}

bad_symbol::bad_symbol(): runtime_error("bad symbol") {}