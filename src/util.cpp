#include "util.hpp"

ReturningType typeFromFunction(std::unique_ptr<Node>& node, ProgramContext& pc) {
    std::vector<Type> types;
    for (auto& param : node->children[1]->children) {
        types.emplace_back(param->children[0], pc);
    }

   return ReturningType(TupleType(types), node->children.size() > 2 ? Type(node->children[2], pc) : Type());
}