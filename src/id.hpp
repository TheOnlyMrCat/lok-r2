#pragma once

#include "clok.hpp"

#include <memory>

typedef std::unique_ptr<class Node> NodePtr;

class Identifier {
public:
    Identifier(NodePtr& node);

private:
    std::vector<strings_t> parts;
};