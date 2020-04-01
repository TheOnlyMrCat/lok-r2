#pragma once

#include <memory>

#include "clok.hpp"
#include "program.hpp"

#include <valuable/value-ptr.hpp>
namespace val=valuable;

typedef std::unique_ptr<class Node> NodePtr;

struct TypeQualifier {
	TypeQualifier(NodePtr& node);
    TypeQualifier(bool, bool, int);
    TypeQualifier(bool, bool, int, TypeQualifier);

    bool operator==(const TypeQualifier& other) const;
    bool operator< (const TypeQualifier& other) const;

	val::value_ptr<TypeQualifier> nested;
	bool isPointer;
    bool forceUpgrade;
	int arraySize;
};

class SingleType {
public:
    SingleType(NodePtr& node, ProgramContext& pc);
    SingleType(Identifier, TypeQualifier);
    SingleType(Identifier);

    bool operator==(const SingleType&) const;
    bool operator< (const SingleType&) const;

    Identifier id;
    val::value_ptr<TypeQualifier> qualifier;
};

class TupleType {
public:
    TupleType(NodePtr& node, ProgramContext& pc);
    TupleType(std::vector<Type>);

    bool operator==(const TupleType&) const;
    bool operator< (const TupleType&) const;

    std::vector<Type> types;
};

class ReturningType {
public:
    ReturningType(NodePtr& node, ProgramContext& pc);
    ReturningType(TupleType, Type);

    bool operator==(const ReturningType&) const;
    bool operator< (const ReturningType&) const;

    TupleType input;
    Type output;
};