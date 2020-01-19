#pragma once

#include "clok.hpp"

struct SingleType;
struct TupleType;
struct ReturningType;

struct ProgramContext;

struct Type {
	Type();
	Type(NodePtr& node, ProgramContext& pc);
	Type(Type&&);
	~Type();

    Type& operator=(Type&&);

    int typeType;
    std::unique_ptr<SingleType> basic;
    std::unique_ptr<TupleType> tuple;
    std::unique_ptr<ReturningType> func;
};

struct Identifier {
	Identifier(NodePtr& node, ProgramContext& context);
	Identifier(std::vector<IdPart> parts);

    std::vector<IdPart> parts;
};

class Symbol {
public:
	Symbol(NodePtr& node, bool isType, ProgramContext& pc);
	Symbol(NodePtr& node, bool isType, ProgramContext& pc, std::vector<IdPart> prefix);

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

struct ProgramContext {
	std::unordered_map<std::string, Identifier> aliases;
	std::vector<IdPart> currentNamespace;
};

class Program {
public:
	void findSymbols(std::unique_ptr<Node>& tree);
	void findDeclarations(std::unique_ptr<Node>& tree);

private:
	std::vector<Symbol> symbols;
	std::vector<Decl> declarations;

	ProgramContext context;

	void _findSymbols(std::unique_ptr<Node>& node);
};
