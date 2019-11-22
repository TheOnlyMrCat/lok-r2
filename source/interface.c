#include <stdio.h>
#include <stdlib.h>

#include "grammar.h"
#include "nodetypes.d"
; // Necessary semicolon to terminate enum declaration

extern FILE *yyin;

int openFile(const char *filename) {
    yyin = fopen(filename, "r");
    return ferror(yyin);
}

void allocResult() {
    parseResult = nalloc(0, NONE);
}

void recursiveFree(Node *n) {
    for (int i = 0; i < n->cCount; i++) {
        recursiveFree(n->children[i]);
    }
    free(n);
}

void freeResult() {
    recursiveFree(parseResult);
}