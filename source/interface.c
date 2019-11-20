#include <stdio.h>
#include <stdlib.h>

#include "grammar.h"

extern FILE *yyin;

int openFile(const char *filename) {
    yyin = fopen(filename, "r");
    return ferror(yyin);
}

void clearResult() {
    parseResult = (Node) {0, 0, 0, 0, 0};
}

void recursiveFree(Node *n) {
    for (int i = 0; i < n->cCount; i++) {
        recursiveFree(n->children[i]);
    }
    free(n);
}

void freeResult() {
    for (int i = 0; i < parseResult.cCount; i++) {
        recursiveFree(parseResult.children[i]);
    }
}