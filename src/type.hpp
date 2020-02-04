#pragma once

#include <memory>

#include "clok.hpp"
#include "program.hpp"

#include <valuable/value-ptr.hpp>
namespace val=valuable;

typedef std::unique_ptr<class Node> NodePtr;

struct TypeQualifier {
	TypeQualifier(NodePtr& node);
    TypeQualifier(bool, int);
    TypeQualifier(bool, int, TypeQualifier);

	val::value_ptr<TypeQualifier> nested;
	bool isPointer;
	int arraySize;
};

class SingleType {
public:
    SingleType(NodePtr& node, ProgramContext& pc);
    SingleType(Identifier, TypeQualifier);
    SingleType(Identifier);

private:
    Identifier id;
    val::value_ptr<TypeQualifier> qualifier;
};

class TupleType {
public:
    TupleType(NodePtr& node, ProgramContext& pc);
    TupleType(std::vector<Type>);

private:
    std::vector<Type> types;
};

class ReturningType {
public:
    ReturningType(NodePtr& node, ProgramContext& pc);
    ReturningType(TupleType, Type);

private:
    TupleType input;
    Type output;
};