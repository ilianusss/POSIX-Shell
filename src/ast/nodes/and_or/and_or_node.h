#ifndef AND_OR_NODE_H
#define AND_OR_NODE_H

// DEFINITIONS
//
// HEADERS
#include "../../../utils/utils.h"
#include "../../ast.h"

// LIBS
#include <stdio.h>
#include <stdlib.h>

// STRUCTURES
struct and_or_node
{
    struct node node;
    struct node *left;
    struct node *right;
    int is_and; // 1 for `&&`, 0 for `||`
};

// FUNCTIONS
struct and_or_node *and_or_node_create(struct node *left, struct node *right,
                                       int is_and);
int and_or_node_exec(void *param);
void and_or_node_destroy(void *param);

#endif /* ! AND_OR_NODE_H */
