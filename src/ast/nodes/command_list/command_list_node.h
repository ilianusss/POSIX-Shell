#ifndef COMMAND_LIST_NODE_H
#define COMMAND_LIST_NODE_H

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
struct command_list_node
{
    struct node node;
    struct node **commands;
};

// FUNCTIONS
struct command_list_node *command_list_node_create(struct node **commands);
int command_list_node_exec(void *param);
void command_list_node_destroy(void *param);

#endif /* ! COMMAND_LIST_NODE_H */
