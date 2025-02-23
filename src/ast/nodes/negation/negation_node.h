#ifndef NEGATION_NODE_H
#define NEGATION_NODE_H

// DEFINITIONS
//
// HEADERS
#include "../../../utils/utils.h"
#include "../../ast.h"

// LIBS
#include <stdio.h>
#include <stdlib.h>

// STRUCTURES
struct negation_node
{
    struct node node;
    struct node *command;
};

// FUNCTIONS
struct negation_node *negation_node_create(struct node *command);
int negation_node_exec(void *param);
void negation_node_destroy(void *param);

#endif /* ! NEGATION_NODE_H */
