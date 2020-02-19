#include "util.hpp"
#include "type.hpp"
#include "program.hpp"
#include "grammar.hpp"

#include <algorithm>

ReturningType typeFromFunction(std::unique_ptr<Node>& node, ProgramContext& pc) {
    std::vector<Type> types;
    for (auto& param : node->children[1]->children) {
        types.emplace_back(param->children[0], pc);
    }

   return ReturningType(TupleType(types), node->children.size() > 2 ? Type(node->children[2], pc) : Type());
}

std::vector<Symbol> aggregateStack(std::vector<StackFrame> frames) {
    std::vector<Symbol> symbols = frames.back().symbols;
    frames.pop_back();
    while (frames.size() > 0) {
        for (auto s : frames.back().symbols) {
            symbols.push_back(s);
        }
        frames.pop_back();
    }
    return symbols;
}

Symbol getSymbol(Identifier id, ProgramContext& pc) {
    auto stack = aggregateStack(pc.stackFrames);
    auto local = std::find_if(stack.begin(), stack.end(), [&id](const Symbol &s) { return s.id == id; });
    if (local != stack.end()) {
        return *local;
    }
    auto alias = pc.aliases.find(id);
    if (alias != pc.aliases.end()) {
        return alias->second;
    }
    auto symbol = std::find_if(pc.symbols.begin(), pc.symbols.end(), [&id](const std::pair<const Identifier, Symbol> &s) { return s.second.id == id; });
    if (symbol != pc.symbols.end()) {
        return symbol->second;
    }
    throw bad_symbol();
}