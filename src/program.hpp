#pragma once

#include "clok.hpp"
#include <map>

#include <valuable/value-ptr.hpp>
namespace val=valuable;

struct SingleType;
struct TupleType;
struct ReturningType;

struct ProgramContext;

struct Type {
	Type();
	Type(NodePtr& node, ProgramContext& pc);
	Type(SingleType);
	Type(TupleType);
	Type(ReturningType);
	~Type();

	Type& operator=(const Type&);
	bool operator==(const Type&) const;

    int typeType;
    val::value_ptr<SingleType> basic;
    val::value_ptr<TupleType> tuple;
    val::value_ptr<ReturningType> func;
};

struct Identifier {
	Identifier(NodePtr& node, ProgramContext& context);
	Identifier(std::vector<IdPart> parts);

	bool operator== (const Identifier &other) const;
	bool operator< (const Identifier &other) const;

    std::vector<IdPart> parts;
};

class Symbol {
public:
	Symbol(NodePtr& node, bool isType, ProgramContext& pc);
	Symbol(Type, Identifier);

	std::string toLokConv();
	std::string toCxxConv();
	std::string toCConv();

	Type type;
	Identifier id;
};

class Decl {
public:
	virtual ~Decl();
};

class Statement {
	virtual class Value *codegen();
};

class Expr : public Statement {
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

class StringValue : public Expr {
public:
	StringValue(std::string);

	std::string value;
};

class FuncValue : public Expr {
public:
	FuncValue(ReturningType, std::vector<Statement*>);
	~FuncValue();

	std::vector<Statement*> statements;
};

class SymbolExpr : public Expr {
public:
	SymbolExpr(Identifier, ProgramContext&);

	Symbol symbol;
};

struct StackFrame {
	std::vector<Symbol> symbols;
};

struct ProgramContext {
	std::map<Identifier, Symbol> aliases;
	std::vector<IdPart> currentNamespace;
	std::vector<StackFrame> stackFrames;
	std::map<Identifier, Symbol> symbols;
	std::vector<Decl> declarations;
};

struct ExtrapSymbol {
	Symbol *s;
	Expr *value;
};

class Program {
public:
	void findSymbols(std::unique_ptr<Node>& tree);
	void extrapolate(std::unique_ptr<Node>& tree);

private:
	ProgramContext context;

	std::vector<ExtrapSymbol> extrapolatedSymbols;

	std::vector<Statement*> _extrapBlock(std::unique_ptr<Node>& node);
	Expr *_extrapolate(std::unique_ptr<Node>& node);
};
