#pragma once

#include <memory>

#include "id.hpp"

typedef std::unique_ptr<class Node> NodePtr;

struct Type {
    int typeType;
    std::unique_ptr<struct SingleType> basic;
    std::unique_ptr<struct TupleType> tuple;
    std::unique_ptr<struct ReturningType> func;
};

struct TypeQualifier {

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