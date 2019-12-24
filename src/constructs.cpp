#include "type.hpp"
#include "id.hpp"

#include "clok.hpp"

Identifier::Identifier(NodePtr& node) {
    for (NodePtr *part = &node->children[0]; (*part)->children.size() > 0; part = &(*part)->children[0]) {
        parts.push_back((*part)->value.valC);
    }
}

SingleType::SingleType(NodePtr& node): id(Identifier(node->children[0])) {
    // TODO: qualifiers
}

TupleType::TupleType(NodePtr& node) {
    for (int i = 0; i < node->children.size(); i++) {
        switch (node->children[i]->type) {
        case NodeType::TYPESINGLE:
            types.push_back({0, std::make_unique<SingleType>(node->children[i]), nullptr, nullptr});
        case NodeType::TYPEMULTI:
            types.push_back({1, nullptr, std::make_unique<TupleType>(node->children[i]), nullptr});
        case NodeType::TYPEFN:
            types.push_back({2, nullptr, nullptr, std::make_unique<ReturningType>(node->children[i])});
        default:
            PLOGF << "Unexpected node type in tuple";
            exit(EXIT_FAILURE);
        }
    }
}

ReturningType::ReturningType(NodePtr& node): input(node->children[0]) {
    switch (node->children[1]->type) {
        case NodeType::TYPESINGLE:
            output = {0, std::make_unique<SingleType>(node->children[1]), nullptr, nullptr};
        case NodeType::TYPEMULTI:
            output = {1, nullptr, std::make_unique<TupleType>(node->children[1]), nullptr};
        case NodeType::TYPEFN:
            output = {2, nullptr, nullptr, std::make_unique<ReturningType>(node->children[1])};
        default:
            PLOGF << "Unexpected node type in returning type";
            exit(EXIT_FAILURE);
    }
}