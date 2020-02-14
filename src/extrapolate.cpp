#include "clok.hpp"
#include "program.hpp"
#include "type.hpp"
#include "util.hpp"

void Program::findSymbols(std::unique_ptr<Node>& tree) {
	for (auto& node : tree->children) {
		if (node->type == NodeType::DECL) {
			Symbol s = Symbol(node, false, context);
			context.symbols.emplace(std::make_pair(s.id, std::move(s)));
		} else if (node->type == NodeType::TYPEDECL) {
			context.currentNamespace.push_back(std::make_pair(strings[node->value.valC], true));
			findSymbols(node->children[0]->children[0]);
			context.currentNamespace.pop_back();
			Symbol s = Symbol(node, true, context);
			context.symbols.emplace(std::make_pair(s.id, std::move(s)));
		} else if (node->type == NodeType::CTORDEF) {
			auto ctorname = std::vector<IdPart>(context.currentNamespace);
			ctorname.emplace_back("new", false);
			Symbol s = Symbol(Type(SingleType(Identifier(context.currentNamespace))), ctorname);
			context.symbols.emplace(std::make_pair(s.id, std::move(s)));
		} else if (node->type == NodeType::DTORDEF) {
			auto dtorname = std::vector<IdPart>(context.currentNamespace);
			dtorname.emplace_back("del", false);
			Symbol s = Symbol(Type(SingleType(Identifier(context.currentNamespace))), dtorname);
			context.symbols.emplace(std::make_pair(s.id, std::move(s)));
		} else if (node->type == NodeType::OPOVERLOAD) {
			auto overloadname = std::vector<IdPart>(context.currentNamespace);
			overloadname.emplace_back(strings[node->value.valC], false);
			Symbol s = Symbol(typeFromFunction(node->children[0], context), overloadname);
			context.symbols.emplace(std::make_pair(s.id, std::move(s)));
		} else if (node->type == NodeType::NAMESPACE) {
			context.currentNamespace = Identifier(node->children[0], context).parts;
			findSymbols(node->children[1]);
			context.currentNamespace.clear();
		}
	}
}

bool checkForOverload(SingleType l, SingleType r, std::string op, ProgramContext &pc) {
	std::vector<IdPart> symbolLoc = l.id.parts;
	symbolLoc.emplace_back(op, false);
	return pc.symbols.find(symbolLoc) != pc.symbols.end();
}

bool checkTuple(TupleType l, TupleType r, std::string op, ProgramContext &pc) {
	bool valid = l.types.size() != r.types.size();
	for (int i = 0; valid && i < l.types.size(); i++) {
		Type lType = l.types[i], rType = r.types[i];
		if (lType.typeType == 2 || rType.typeType == 2) {
			valid = false;
		} else if (lType.typeType == 1) {
			if (rType.typeType != 1) {
				valid = false;
			} else {
				valid = checkTuple(*lType.tuple, *rType.tuple, op, pc);
			}
		} else {
			if (rType.typeType != 0) {
				valid = false;
			} else {
				valid = checkForOverload(*lType.basic, *rType.basic, op, pc);
			}
		}
	}
	return valid;
}

// node is expected to be one of the EXPR node types
Expr *Program::_extrapolate(std::unique_ptr<Node>& node) {
	switch (node->type) {
		case NodeType::EXPRBASIC: {
			Expr *left = _extrapolate(node->children[0]);
			Expr *right = _extrapolate(node->children[1]);
			Type lType = left->type;
			Type rType = right->type;
			bool valid = true;
			std::string op = strings[node->value.valC];
			if (lType.typeType == 1) {
				if (rType.typeType != 1) {
					PLOGE << "Bad"; //TODO link error with yy::parser::error
					throw; //TODO Throw something
				}
				valid = checkTuple(*lType.tuple, *rType.tuple, op, context);
			} else if (lType.typeType == 0) {
				if (rType.typeType != 0) {
					PLOGE << "Disappointing"; //TODO see above
					throw;
				}
				valid = checkForOverload(*lType.basic, *rType.basic, op, context);
			}
			return new OpExpr(lType, left, right, strings[node->value.valC]);
		}
		case NodeType::VALINT:
			return new IntValue(node->value.valI, 64);
		case NodeType::VALFLOAT:
			return new FloatValue(node->value.valF, 64);
		case NodeType::VALBIT:
			return new BitValue(node->value.valB);
		default:
			PLOGE << "Unhandled expression type";
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