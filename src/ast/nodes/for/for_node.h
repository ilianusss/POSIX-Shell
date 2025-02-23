#ifndef FOR_NODE_H
#define FOR_NODE_H

// DEFINITIONS
//
// HEADERS
#include "../../../cmds/var/expand_var.h"
#include "../../../utils/utils.h"
#include "../../ast.h"

// LIBS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// STRUCTURES
struct for_node
{
    struct node node;
    char *var;
    char **items;
    int item_count;
    struct node *body;
};

// FUNCTIONS
struct for_node *for_node_create(char *loop_var, char **items, int item_count,
                                 struct node *body);
int for_node_exec(void *param);
void for_node_destroy(void *param);

#endif /* ! FOR_NODE_H */
