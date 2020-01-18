#pragma once

#include <memory>

#include "clok.hpp"
#include "program.hpp"

typedef std::unique_ptr<class Node> NodePtr;

struct TypeQualifier {
	TypeQualifier(NodePtr& node);

	std::unique_ptr<TypeQualifier> nested;
	bool isPointer;
	int arraySize;
};

class SingleType {
public:
    SingleType(NodePtr& node, ProgramContext& pc);

private:
    Identifier id;
    TypeQualifier qualifier;
};

class TupleType {
public:
    TupleType(NodePtr& node, ProgramContext& pc);

private:
    std::vector<Type> types;
};

class ReturningType {
public:
    ReturningType(NodePtr& node, ProgramContext& pc);

private:
    TupleType input;
    Type output;
};