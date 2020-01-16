#pragma once

#include <memory>

#include "clok.hpp"

typedef std::unique_ptr<class Node> NodePtr;

struct TypeQualifier {
	TypeQualifier(NodePtr& node);

	std::unique_ptr<TypeQualifier> nested;
	bool isPointer;
	int arraySize;
};

class SingleType {
public:
    SingleType(NodePtr& node);

private:
    Identifier id;
    TypeQualifier qualifier;
};

class TupleType {
public:
    TupleType(NodePtr& node);

private:
    std::vector<Type> types;
};

class ReturningType {
public:
    ReturningType(NodePtr& node);

private:
    TupleType input;
    Type output;
};