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

Statement *Program::_extrapStmt(std::unique_ptr<Node>& node) {
	Statement *s;
	switch (node->type) {
		case NodeType::DECL: {
			PLOGD << "A variable declaration";
			Identifier id = Identifier({{strings[node->value.valC], false}});
			Type type;
			int expressionIndex = 0;
			if (node->children[0]->type == NodeType::TYPESINGLE || node->children[0]->type == NodeType::TYPEMULTI || node->children[0]->type == NodeType::TYPEFN) {
				type = Type(node->children[0], context);
				expressionIndex = 1;
			}
			if (node->children.size() <= expressionIndex || node->children[expressionIndex]->type == NodeType::ATTRS) {
				Expr *expr = _extrapolate(node->children[expressionIndex]);
				if (type.typeType != -1 && !(expr->type == type)) {
					PLOGE << "No bad type doesn't match";
					throw;
				} else {
					type = expr->type;
				}
				return new DeclStmt(id, type, expr);
			} else {
				context.stackFrames.back().symbols.emplace_back(type, id);
				return new DeclStmt(id, type, nullptr);
			}
		}
		case NodeType::BLOCK: {
			std::vector<Statement*> statements;
			for (auto& n : node->children) {
				statements.push_back(_extrapStmt(n));
			}
			return new BlockStmt(statements);
		}
		case NodeType::STMTDOWHILE:
		case NodeType::STMTWHILE:
			PLOGD << "A while statement, containing...";
			s = new WhileStmt(_extrapolate(node->children[0]), _extrapStmt(node->children[0]), node->type == NodeType::STMTDOWHILE);
			PLOGD << "... (while) nothing else";
			return s;
		case NodeType::STMTDOFOR:
			PLOGD << "A for statement, containing...";
			s = new ForStmt(node->children.size() > 2 ? _extrapStmt(node->children[2]) : nullptr, _extrapolate(node->children[0]->children[0]), _extrapolate(node->children[0]->children[1]), _extrapStmt(node->children[1]), true);
			PLOGD << "... (for) nothing else";
			return s;
		case NodeType::STMTFOR:
			PLOGD << "A for statement, containing...";
			s = new ForStmt(_extrapStmt(node->children[0]->children[0]), _extrapolate(node->children[0]->children[1]), _extrapolate(node->children[0]->children[2]), _extrapStmt(node->children[1]), false);
			PLOGD << "... (for) nothing else";
			return s;
		case NodeType::STMTRTN:
			PLOGD << "A return statement:";
			return new ReturnStmt(_extrapolate(node->children[0]));
		case NodeType::STMTIF:
			PLOGD << "An if statement, containing...";
			s = new IfStmt(_extrapolate(node->children[0]), _extrapStmt(node->children[1]), node->children.size() > 2 ? _extrapStmt(node->children[2]) : nullptr);
			PLOGD << "... (if) nothing else";
			return s;
		case NodeType::EXPRBASIC:
		case NodeType::FUNCDEF:
		case NodeType::VALSTR:
		case NodeType::VALINT:
		case NodeType::VALFLOAT:
		case NodeType::VALBIT:
		case NodeType::FQUALPATH:
			return _extrapolate(node);
			break;
		case NodeType::NONE:
			return nullptr;
		default:
			PLOGF << "Unhandled statement type";
			throw;
	}
}

// node is expected to be something that can parse into an expression
Expr *Program::_extrapolate(std::unique_ptr<Node>& node) {
	switch (node->type) {
		case NodeType::FUNCDEF: {
			PLOGD << "A function definition, containing...";
			ReturningType type = typeFromFunction(node, context);
			Type returnedType;
			StackFrame frame;
			for (auto& param : node->children[1]->children) {
				frame.symbols.emplace_back(Type(param->children[0], context), Identifier({{strings[param->value.valC], false}}));
			}
			
			context.stackFrames.emplace_back(frame);
			Statement *s = _extrapStmt(node->children[0]);
			context.stackFrames.pop_back();
			PLOGD << "... (func) nothing else";
			return new FuncValue(type, s);
		}
		case NodeType::EXPRBASIC: {
			PLOGD << "A basic expression, containing...";
			Expr *left = _extrapolate(node->children[0]);
			if (node->children.size() > 1) {
				Expr *right = _extrapolate(node->children[1]);
				Type lType = left->type;
				Type rType = right->type;
				bool valid = true;
				std::string op = strings[node->value.valC];
				PLOGD << "... (expr) with operator " << op;
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
			} else {
				std::string op = strings[node->value.valC];
				PLOGD << "... (expr) with " << (node->value.valB ? "postfix" : "prefix") << " operator " << op;
				//TODO return something and check for overloads
				return nullptr;
			}
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
		case NodeType::NONE:
			return nullptr;
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
			if (node->children.size() < expressionIndex) {
				Expr *extrapolated = _extrapolate(node->children[expressionIndex]);
				if (expectedType.typeType != -1 && !(extrapolated->type == expectedType)) {
					//TODO check for conversion
					PLOGE << "No bad type doesn't match";
					throw; //TODO see above
				}
				Symbol *s = &context.symbols.find(Symbol(node, false, context).id)->second;
				s->type = expectedType;
				extrapolatedSymbols.push_back({s, extrapolated});
			}
		} else if (node->type == NodeType::NAMESPACE) {
			context.currentNamespace = Identifier(node->children[0], true, context).parts;
			extrapolate(node->children[1]);
			context.currentNamespace.clear();
		} else if (node->type == NodeType::RUN) {
			extrapolatedSymbols.push_back({new Symbol(typeFromFunction(node->children[0], context), Identifier({{"run", false}, {"0", false}})), _extrapolate(node->children[0]), true});
		} else if (node->type == NodeType::TYPEDECL) {
			auto oldNamespace = std::vector<IdPart>(context.currentNamespace);
			context.currentNamespace.push_back({strings[node->value.valC], true});
			extrapolate(node->children[0]->children[0]);
			context.currentNamespace = oldNamespace;
		} else if (node->type == NodeType::OPOVERLOAD) {
			auto overloadname = std::vector<IdPart>(context.currentNamespace);
			overloadname.emplace_back(strings[node->value.valC], false);
			Symbol *s = &context.symbols.find(Identifier(overloadname))->second;
			extrapolatedSymbols.push_back({s, _extrapolate(node->children[0])});
		}
		//  else if (node->type == NodeType::CTORDEF) {
		// 	auto ctorname = std::vector<IdPart>(context.currentNamespace);
		// 	ctorname.emplace_back("new", false);
		// 	Symbol *s = &context.symbols.find(Identifier(ctorname))->second;
		// 	extrapolatedSymbols.push_back({s, _extrapolate()})
		// }
		//TODO
	}
}