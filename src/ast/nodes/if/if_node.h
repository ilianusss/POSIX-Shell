#ifndef IF_NODE_H
#define IF_NODE_H

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
struct if_node
{
    struct node node;
    struct node *condition;
    struct node *then_branch;
    struct node *else_branch;
};

// FUNCTIONS
struct if_node *if_node_create(struct node *condition, struct node *then_branch,
                               struct node *else_branch);
int if_node_exec(void *param);
void if_node_destroy(void *param);

#endif /* ! IF_NODE_H */
