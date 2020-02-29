#include "clok.hpp"
#include "program.hpp"
#include "type.hpp"
#include "util.hpp"
#include "grammar.hpp"

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
			context.currentNamespace = Identifier(node->children[0], true, context).parts;
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

std::vector<Statement*> Program::_extrapBlock(std::unique_ptr<Node>& node) {
	std::vector<Statement*> statements;
	for (auto& sn : node->children) {
		switch (sn->type) {
			case NodeType::EXPRBASIC:
			case NodeType::FUNCDEF:
			case NodeType::VALSTR:
			case NodeType::VALINT:
			case NodeType::VALFLOAT:
			case NodeType::VALBIT:
			case NodeType::FQUALPATH:
				statements.push_back(static_cast<Statement*>(_extrapolate(sn)));
				break;
			default:
				PLOGF << "Unhandled statement type";
				throw;
		}
	}
	return statements;
}

// node is expected to be one of the EXPR node types
Expr *Program::_extrapolate(std::unique_ptr<Node>& node) {
	switch (node->type) {
		case NodeType::FUNCDEF: {
			PLOGD << "A function definition, containing...";
			ReturningType type = typeFromFunction(node, context);
			Type returnedType;
			std::vector<Statement*> s;
			StackFrame frame;
			for (auto& param : node->children[1]->children) {
				frame.symbols.emplace_back(Type(param->children[0], context), Identifier({{strings[param->value.valC], false}}));
			}
			
			context.stackFrames.emplace_back(frame);
			if (node->children[0]->type == NodeType::BLOCK) {
				s = _extrapBlock(node->children[0]);
			} else {
				s = {static_cast<Statement*>(_extrapolate(node->children[0]))};
			}
			context.stackFrames.pop_back();
			PLOGD << "...nothing else";
			return new FuncValue(type, s);
		}
		case NodeType::EXPRBASIC: {
			PLOGD << "A basic expression, containing...";
			Expr *left = _extrapolate(node->children[0]);
			Expr *right = _extrapolate(node->children[1]);
			Type lType = left->type;
			Type rType = right->type;
			bool valid = true;
			std::string op = strings[node->value.valC];
			PLOGD << "...with operator " << op;
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
				valid = checkForOverload(*lType.basic, *rType.basic, op, context); //TODO get type returned
			}
			return new OpExpr(lType, left, right, strings[node->value.valC]);
		}
		case NodeType::VALSTR:
			PLOGD << "A string literal";
			return new StringValue(strings[node->value.valC]);
		case NodeType::VALINT:
			PLOGD << "An integer literal";
			return new IntValue(node->value.valI, 64);
		case NodeType::VALFLOAT:
			PLOGD << "A float literal";
			return new FloatValue(node->value.valF, 64);
		case NodeType::VALBIT:
			PLOGD << "A bit literal";
			return new BitValue(node->value.valB);
		case NodeType::FQUALPATH:
			PLOGD << "A symbol reference to...";
			return new SymbolExpr(Identifier(node, true, context), context);
		default:
			PLOGF << "Unhandled expression type";
			throw; //TODO see above
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
			Expr *extrapolated = _extrapolate(node->children[expressionIndex]);
			if (expectedType.typeType != -1 && !(extrapolated->type == expectedType)) {
				//TODO check for conversion
				PLOGE << "No bad type doesn't match";
				throw; //TODO see above
			}
			Symbol *s = &context.symbols.find(Symbol(node, false, context).id)->second;
			s->type = expectedType;
			extrapolatedSymbols.push_back({s, extrapolated});
		} else if (node->type == NodeType::NAMESPACE) {
			context.currentNamespace = Identifier(node->children[0], true, context).parts;
			extrapolate(node->children[1]);
			context.currentNamespace.clear();
		}
	}
}