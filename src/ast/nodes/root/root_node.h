#ifndef ROOT_NODE_H
#define ROOT_NODE_H

// DEFINITIONS
//
// HEADERS
#include "../../../utils/utils.h"
#include "../../ast.h"

// LIBS
#include <stdio.h>
#include <stdlib.h>

// STRUCTURES
struct root_node
{
    struct node node;
    struct node **commands;
    size_t count;
};

// FUNCTIONS
struct root_node *root_node_create(struct node **child, size_t count);
int root_node_exec(void *param);
void root_node_destroy(void *param);

#endif /* ! ROOT_NODE_H */
