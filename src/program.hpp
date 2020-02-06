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
	Type(SingleType);
	Type(TupleType);
	Type(ReturningType);
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
	Symbol(Type, Identifier);

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
	OpExpr(Type, Expr*, Expr*, std::string);
	~OpExpr() override;

	Expr *left;
	Expr *right;
	std::string op;
};

class AssigExpr : public Expr {
public:
	Symbol variable;
	Expr *right;
};

class CompAssigExpr : public Expr {
public:
	Symbol variable;
	Expr *right;
	std::string op;
};

class IntValue : public Expr {
public:
	IntValue(long long, int);

	long long value;
};

class FloatValue : public Expr {
public:
	FloatValue(double, int);

	double value;
};

class BitValue : public Expr {
public:
	BitValue(bool);

	bool value;
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

	Expr *_extrapolate(std::unique_ptr<Node>& node);
};
