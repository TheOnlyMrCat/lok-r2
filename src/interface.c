#include <stdio.h>
#include <stdlib.h>

#include "grammar.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

extern FILE *yyin;

int openFile(const char *filename) {
    yyin = fopen(filename, "r");
    return ferror(yyin);
}

void allocResult() {
    parseResult = nalloc(0, 0);
}

void recursiveFree(Node *n) {
    for (int i = 0; i < n->cCount; i++) {
        recursiveFree(n->children[i]);
        n->children[i] = NULL;
    }
    free(n);
}

void freeResult() {
    recursiveFree(parseResult);
}