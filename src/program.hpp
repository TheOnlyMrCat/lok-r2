#pragma once

#include "clok.hpp"

#include <valuable/value-ptr.hpp>
namespace val=valuable;

struct SingleType;
struct TupleType;
struct ReturningType;

struct ProgramContext;

struct Type {
	Type();
	Type(NodePtr& node, ProgramContext& pc);
	Type(Type&&);
	Type(const Type&);
	~Type();

    Type& operator=(Type&&);

    int typeType;
    val::value_ptr<SingleType> basic;
    val::value_ptr<TupleType> tuple;
    val::value_ptr<ReturningType> func;
};

struct Identifier {
	Identifier(NodePtr& node, ProgramContext& context);
	Identifier(std::vector<IdPart> parts);

    std::vector<IdPart> parts;
};

class Symbol {
public:
	Symbol(NodePtr& node, bool isType, ProgramContext& pc);

	std::string toLokConv();
	std::string toCxxConv();
	std::string toCConv();

private:
	Type type;
	Identifier id;
};

class Decl {
public:
	virtual ~Decl();
};

class Expr {
public:
	Expr(Type);
	virtual ~Expr();
	Type type;
};

class OpExpr : public Expr {
public:
	OpExpr(Type, val::value_ptr<Expr>, val::value_ptr<Expr>, std::string);

	val::value_ptr<Expr> left;
	val::value_ptr<Expr> right;
	std::string op;
};

class AssigExpr : public Expr {
public:
	Symbol variable;
	std::unique_ptr<Expr> right;
};

class CompAssigExpr : public Expr {
public:
	Symbol variable;
	std::unique_ptr<Expr> right;
	std::string op;
};

struct ProgramContext {
	std::unordered_map<std::string, Identifier> aliases;
	std::vector<IdPart> currentNamespace;
};

class Program {
public:
	void findSymbols(std::unique_ptr<Node>& tree);
	void extrapolate(std::unique_ptr<Node>& tree);

private:
	std::vector<Symbol> symbols;
	std::vector<Decl> declarations;

	ProgramContext context;

	std::unique_ptr<Expr> _extrapolate(std::unique_ptr<Node>& node);
};
