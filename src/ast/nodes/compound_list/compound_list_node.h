#ifndef COMPOUND_LIST_NODE_H
#define COMPOUND_LIST_NODE_H

// DEFINITIONS
//
// HEADERS
#include "../../../utils/utils.h"
#include "../../ast.h"

// LIBS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// STRUCTURES
struct compound_list_node
{
    struct node node;
    struct node **commands;
};

// FUNCTIONS
struct compound_list_node *compound_list_node_create(struct node **commands);
int compound_list_node_exec(void *param);
void compound_list_node_destroy(void *param);

#endif /* ! COMPOUND_LIST_NODE_H */
