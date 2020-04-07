#pragma once

#include "clok.hpp"
#include <map>
#include <unordered_map>

#include <valuable/value-ptr.hpp>
namespace val=valuable;

class Symbol;
struct SingleType;
struct TupleType;
struct ReturningType;

struct Value;

struct ProgramContext;

struct TypeRef {
	TypeRef(Identifier);

	std::vector<Symbol>
};

struct Type {
	Type();
	Type(NodePtr& node, ProgramContext& pc);
	Type(SingleType);
	Type(TupleType);
	Type(ReturningType);
	~Type();

	Type& operator=(const Type&);
	bool operator==(const Type&) const;
	bool operator< (const Type&) const;

    int typeType;
    val::value_ptr<SingleType> basic;
    val::value_ptr<TupleType> tuple;
    val::value_ptr<ReturningType> func;
};

struct Identifier {
	Identifier(NodePtr& node, bool ignoreCurrentNamespace, ProgramContext& context);
	Identifier(std::vector<IdPart> parts);

	bool operator== (const Identifier &other) const;
	bool operator< (const Identifier &other) const;

    std::vector<IdPart> parts;
};

struct Attributes {
	std::unordered_map<std::string, std::string> attrs;
};

class Symbol {
public:
	Symbol(NodePtr& node, bool isType, ProgramContext& pc);
	Symbol(Type, Identifier);
	Symbol(Type, Identifier, Attributes);

	void fillAttributes(NodePtr&);

	std::string toLokConv();
	std::string toCxxConv();
	std::string toCConv();

	Type type;
	Identifier id;
	Attributes attr;
};

class Decl {
public:
	virtual ~Decl();
};

class Statement {
	virtual Value *codegen();
};

class Expr : public Statement {
public:
	Expr(Type);
	virtual ~Expr();
	Type type;
};

class DeclStmt : public Statement {
public:
	DeclStmt(Identifier, Type, Expr*);

	Identifier id;
	Type type;
	Expr* value;
};

class BlockStmt : public Statement {
public:
	BlockStmt(std::vector<Statement*>);

	std::vector<Statement*> statements;
};

class IfStmt : public Statement {
public:
	IfStmt(Expr*, Statement*, Statement*);

	Expr *ifexpr;
	Statement* iftrue;
	Statement* ifnot;
};

class ReturnStmt : public Statement {
public:
	ReturnStmt(Expr*);

	Expr *expr;
};

class WhileStmt : public Statement {
public:
	WhileStmt(Expr*, Statement*, bool);

	Expr *cond;
	Statement *body;
	bool isDo;
};

class ForStmt : public Statement {
public:
	ForStmt(Statement*, Expr*, Expr*, Statement*, bool);

	Statement* decl;
	Expr* cond;
	Expr* inc;
	Statement* body;
	bool isDo;
};

class ArgsExpr : public Expr {
public:
	ArgsExpr(std::vector<Expr*>);
	~ArgsExpr() override;

	std::vector<Expr*> expressions;
};

class CallExpr : public Expr {
public:
	CallExpr(Expr*, ArgsExpr);
	~CallExpr();

	Expr *expr;
	ArgsExpr args;
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
	FuncValue(ReturningType, Statement*);
	~FuncValue();

	Statement *statement;
};

class SymbolExpr : public Expr {
public:
	SymbolExpr(Identifier, ProgramContext&);
	SymbolExpr(Symbol);

	Symbol symbol;
};

struct StackFrame {
	std::vector<Symbol> symbols;
};

struct ProgramContext {
	std::vector<IdPart> currentNamespace;
	std::vector<Identifier> types;
	std::map<Identifier, Symbol> symbols;
	std::map<std::string, std::map<Identifier, Symbol>> externalSymbols;

	std::multimap<Type, Symbol> forcedConstructors;
	std::multimap<std::string, Symbol> opOverloads;

	std::vector<Decl> declarations;
	std::vector<StackFrame> stackFrames;
};

struct ExtrapSymbol {
	void destroy();

	Symbol *s;
	Statement *value;
	bool destroySymbol = false;
};

class Program {
public:
	void locateTypes(std::unique_ptr<Node>& tree);
	void findSymbols(std::unique_ptr<Node>& tree);
	void extrapolate(std::unique_ptr<Node>& tree, std::unordered_map<std::string, Program>& otherProgram, std::string workingDir);

private:
	ProgramContext context;

	std::vector<ExtrapSymbol> extrapolatedSymbols;

	Statement *_extrapStmt(std::unique_ptr<Node>& node);
	Expr *_extrapolate(std::unique_ptr<Node>& node);
};
