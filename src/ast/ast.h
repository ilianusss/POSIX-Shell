#ifndef AST_H
#define AST_H
// DEFINITIONS
//
// HEADERS
//
// LIBS
#include <stdio.h>

// STRUCTURES
struct vtable
{
    int (*exec)(void *param); // Function pointer for execution
    void (*free)(void *param); // Function pointer for freeing
};

struct node
{
    struct vtable *table;
};

// FUNCTIONS
//
#endif /* ! AST_H */
