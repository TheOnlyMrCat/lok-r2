#pragma once

#include <string>
#include <memory>
#include <utility>
#include <vector>

#include <plog/Log.h>

#include "grammar.hpp"

typedef std::pair<std::string, bool> IdPart;

extern std::unique_ptr<Node> parseResult;
extern std::vector<std::string> strings;

strings_t getString(std::string string);

int parse();
void dumpAST(std::unique_ptr<Node>& root, std::string file);

struct SingleType;
struct TupleType;
struct ReturningType;

struct Type {
	Type();
	Type(NodePtr& node);
	Type(Type&&);
	~Type();

    Type& operator=(Type&&);

    int typeType;
    std::unique_ptr<SingleType> basic;
    std::unique_ptr<TupleType> tuple;
    std::unique_ptr<ReturningType> func;
};

struct Identifier {
	Identifier(NodePtr& node); //! Current implementation requires resolution of types
	Identifier(std::vector<IdPart> parts);

private:
    std::vector<IdPart> parts;
};

class Symbol {
public:
	Symbol(NodePtr& node, bool isType);
	Symbol(NodePtr& node, bool isType, std::vector<IdPart> prefix);

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

class Program {
public:
	void findSymbols(std::unique_ptr<Node>& tree);
	void findDeclarations(std::unique_ptr<Node>& tree);

private:
	std::vector<Symbol> symbols;
	std::vector<Decl> declarations;

	void _findSymbols(std::unique_ptr<Node>& node, std::vector<IdPart> prefix);
};
